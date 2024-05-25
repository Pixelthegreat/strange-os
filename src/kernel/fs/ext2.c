#include "ext2.h"
#include "fs.h"
#include "../panic.h"
#include "../disk/ata.h"
#include "../disk/mbr.h"
#include "../kprint.h"
#include "../dev.h"
#include "../../include/string.h"
#include "../heap.h"

/* flags */
#define EXT2_INIT (1 << 0)
#define EXT2_ERR  (1 << 1)

struct ext2_bgd_info {
	u8 *bub; /* block usage bitmap */
	u8 *iub; /* inode usage bitmap */
};

/* for pointer stuff */
struct ext2_sb_info {
	struct ext2_sb *sb; /* superblock */
	struct dev_part pt; /* device and partition info */
	struct ext2_bgd *bgdt; /* block group descriptor table */
	int bpg; /* number of block groups */
	u32 bs; /* block size */
	struct ext2_bgd_info *bgds; /* block group descriptor info */
	struct ext2_inode *rootdir; /* root directory */
	char *rootdir_buf; /* root directory buffer (for dirents) */
};

/* list of stuff */
static struct ext2_sb_info ext2_sb_info[8]; /* 8 ext fs's open at once i guess for the time being or whatever */
static int ext2_sb_info_n = 0;
static int ext2_sb_info_c = 8;

static char *randbuf = NULL; /* buffer for storing stuff */

struct fs_info ext2_fs_info = {
	.rd = ext2_readdir,
	.fd = ext2_finddir,
};

/* get block group of inode */
#define EXT2_BLOCKGROUP(e, inode) (((inode) - 1) / (e)->sb->nipg)
#define EXT2_INDEX(e, inode) (((inode) - 1) % (e)->sb->nipg)
#define EXT2_CONT_BLOCK(e, idx) (((idx) * 128) / (e)->bs)

/* functions */
static int ext2_load_bgdt(struct ext2_sb_info *);
static int ext2_read_inode(struct ext2_sb_info *, int, struct ext2_inode *);
static char *ext2_read_data(struct ext2_sb_info *, struct ext2_inode *);
static struct ext2_dirent *ext2_inode_readdir(struct ext2_sb_info *, struct ext2_inode *, u32);
static struct ext2_sb_info *ext2_sb_get_part(kdev_t, int);

/* check ext2 signatures of devices */
extern int ext2_check_sig(int dev) {

	/* main variable for ext2 info */
	int ext2fs = 0; /* masks: 0b0001 = 1, 0b0010 = 2, 0b0100 = 3, 0b1000 = 4 */
	
	/* load boot sector of device */
	char buf[512];
	dev_read_lba(dev, 0, 1, (void *)buf);
	
	/* loop through partitions */
	for (int i = 1; i < 5; i++) {
	
		struct mbr_part_entry pe;
		
		/* load partition table */
		pe = mbr_get_part_entry_buf(buf, i);
		
		/* linux partition */
		if (pe.p_type == MBR_TYPE_LINUX) {

			/* load what should be the superblock of the linux fs */
			char sbb[1024];
			struct ext2_sb *sb = (struct ext2_sb *)&sbb;
			dev_read_lba(dev, pe.lba_addr + 2, 2, (void *)&sbb);

			/* check signature */
			if (sb->sig == 0xef53|| sb->sig == 0x53ef) {

				/* print message */
				kprint("[ext2fs] found ext2 signature on device ");
				kprinthex(dev);
				kprintnl();
				
				/* create superblock entry */
				struct ext2_sb_info *e = &ext2_sb_info[ext2_sb_info_n++];
				e->pt.pe = pe;
				e->pt.p = i;
				e->pt.dev = dev;
				e->bs = 1024 << sb->bs; /* get block size */
				e->rootdir = NULL; /* root directory inode */
				e->rootdir_buf = NULL; /* root directory inode data buffer */
				
				/* copy superblock data */
				struct ext2_sb *sb = (struct ext2_sb *)kmalloc(1024);
				memcpy(sb, sbb, 1024);
				e->sb = sb;

				/* load block group descriptor table */
				ext2_load_bgdt(e);

				/* set value */
				ext2fs |= (1 << (i-1));

				/* set root */
				if (!fs_root->ptr) {
					
					struct fs_node *nroot = fs_alloc(NULL);
					nroot->flags = FS_DIR;
					nroot->inode = 2;
					nroot->dev = dev;
					nroot->part = i;
					nroot->fs = &ext2_fs_info;
					fs_root->ptr = nroot;
				}
			}
		}
	}
	
	return ext2fs;
}

/* load block */
static int ext2_read_block(struct ext2_sb_info *e, ext2_block_t b, void *buf) {

	/* calculate lba sector */
	int s = e->pt.pe.lba_addr + b * (e->bs / 512);
	
	/* read data */
	dev_read_lba(e->pt.dev, s, (e->bs / 512), buf);
	
	return 0;
}

/* load block group descriptor table */
static int ext2_load_bgdt(struct ext2_sb_info *e) {

	/* get block number for start of bgdt */
	ext2_block_t b = !(e->sb->bs)? 2: 1;

	/* get number of block groups */
	int bpg = e->sb->blocks / e->sb->nbpg + 1;
	
	/* get size of buffer in blocks */
	int bufsiz = ALIGN(bpg, e->bs);
	
	/* create buffer */
	void *buf = kmalloc(bufsiz);
	
	/* read blocks */
	for (int i = b; i < (bufsiz / e->bs) + b; i++) {

		ext2_read_block(e, i, buf);
	}
	
	/* set values */
	e->bgdt = (struct ext2_bgd *)buf;
	e->bpg = bpg;
	
	/* load each bitmap and inode table stuffs */
	e->bgds = (struct ext2_bgd_info *)kmalloc(sizeof(struct ext2_bgd_info) * bpg);
	
	for (int i = 0; i < bpg; i++) {

		/* allocate maps */
		e->bgds->bub = (u8 *)kmalloc(e->bs);
		e->bgds->iub = (u8 *)kmalloc(e->bs);
		
		/* read block usage bitmap */
		ext2_read_block(e, e->bgdt[i].bub, e->bgds->bub);
		ext2_read_block(e, e->bgdt[i].iub, e->bgds->iub);
	}
}

/* read data of inode */
static int ext2_read_inode(struct ext2_sb_info *e, int inode, struct ext2_inode *buf) {

	/* get index, block group, and containing block */
	int bg = EXT2_BLOCKGROUP(e, inode);
	int idx = EXT2_INDEX(e, inode);
	int cont = e->bgdt[bg].itb + EXT2_CONT_BLOCK(e, idx);
	
	/* read block */
	if (randbuf == NULL) randbuf = (char *)kmalloc(e->bs);
	ext2_read_block(e, cont, randbuf);
	
	/* copy buffer contents */
	int i2 = idx % (e->bs / 128);
	memcpy((void *)buf, ((void *)randbuf) + i2 * 128, 128);
	
	return 0;
}

/* read data of inode (file data, not like previous) */
static char *ext2_read_data(struct ext2_sb_info *e, struct ext2_inode *p) {
	
	/* todo: add support for the singly indirect, doubly indirect, and triply indirect block pointers */
	if (p->sz_low >= 1024 * 12)
		return NULL;
	
	/* create buffer */
	char *buf = (char *)kmalloc(ALIGN(p->sz_low, 1024));
	
	if (buf == NULL) return NULL;
	
	/* load data */
	int i = 0;
	while (i * 1024 < p->sz_low) {

		ext2_read_block(e, *(&p->dbp0 + i), buf + i * 1024);
		i++;
	}
	
	/* return buffer */
	return buf;
}

/* find file in directory */
extern struct fs_node *ext2_finddir(struct fs_node *n, char *name) {

	/* get partition */
	struct ext2_sb_info *e;
	if ((e = ext2_sb_get_part(n->dev, n->part)) == NULL)
		return NULL;
	
	/* read inode */
	char _in[128];
	struct ext2_inode *p = (struct ext2_inode *)_in;

	ext2_read_inode(e, n->inode, p);

	/* check type */
	if (!(p->tp & EXT2_DIR))
		return NULL;

	/* get inode buffer */
	char *buf = ext2_read_data(e, p);

	/* loop */
	struct fs_node *nd = NULL;
	int i = 0;
	u32 x = 0;
	char namebuf[128];

	while (nd == NULL) {

		struct ext2_dirent *de = (struct ext2_dirent *)&buf[x];
		if (!de->inode && x > ALIGN(p->sz_low, 1024) - 1024) break;
		else if (!de->inode) x = ALIGN(x, 1024);
		else x += de->esz;
		i++;

		/* copy name */
		memcpy(namebuf, de->name, de->nl);
		namebuf[de->nl] = 0;
		
		/* compare */
		if (kstreq(namebuf, name)) {

			nd = fs_alloc(n);
			strcpy(nd->name, namebuf);
			nd->inode = de->inode;

			/* read inode */
			struct ext2_inode *p = (struct ext2_inode *)namebuf;
			ext2_read_inode(e, de->inode, p);
			nd->flags = 0;
			if (p->tp & EXT2_DIR) nd->flags = FS_DIR;
			else if (p->tp & EXT2_REG) nd->flags = FS_FILE;
		}
	}

	/* free buffer */
	kfree(buf);
	return nd;
}

/* read directory entry for inode */
extern struct dirent *ext2_readdir(struct fs_node *n, u32 idx) {

	/* get partition */
	struct ext2_sb_info *e;
	if ((e = ext2_sb_get_part(n->dev, n->part)) == NULL)
		return NULL;

	/* read inode */
	char _in[128];
	struct ext2_inode *p = (struct ext2_inode *)_in;
	
	ext2_read_inode(e, n->inode, p);

	/* check type */
	if (!(p->tp & EXT2_DIR))
		return NULL;
	
	/* get inode buffer */
	char *buf = ext2_read_data(e, p);
	
	/* loop */
	struct dirent *nd = NULL;
	int i = 0, x = 0;
	
	while (i < idx + 1) {

		/* get dirent */
		struct ext2_dirent *de = (struct ext2_dirent *)&buf[x];

		int ib = !de->inode || de->esz > (de->nl + 8);

		/* at the end */
		if (x >= p->sz_low) {
			break;
		}
		
		//else if (ib) x = ALIGN(x, 1024);
		//else x += de->esz;
		x += de->esz;
		
		/* create directory entry */
		if (i == idx) {

			nd = (struct dirent *)kmalloc(sizeof(struct dirent));
			memset(nd, 0, sizeof(struct dirent));
			memcpy(nd->name, de->name, de->nl);
			nd->inode = de->inode;
		}
		i++;
	}
	
	/* free buffer */
	kfree(buf);
	
	/* return directory entry */
	return nd;
}

/* read directory entry from already read inode struct */
static struct ext2_dirent *ext2_inode_readdir(struct ext2_sb_info *e, struct ext2_inode *p, u32 id) {

	/* check inode type */
	if (!(p->tp & EXT2_DIR))
		return NULL;
	
	/* read block data */
	char *buf = NULL;
	
	if (id == 2 && e->rootdir_buf) buf = e->rootdir_buf;
	else buf = ext2_read_data(e, p);
	
	/* set buffer */
	if (id == 2 && !(e->rootdir_buf)) {

		e->rootdir = p;
		e->rootdir_buf = buf;
	}
	//
}

/* get partition info */
static struct ext2_sb_info *ext2_sb_get_part(kdev_t dev, int p) {
	
	struct ext2_sb_info *e = NULL;
	
	/* loop through list */
	for (int i = 0; i < ext2_sb_info_n; i++) {

		/* check device and partition numbers */
		if (ext2_sb_info[i].pt.dev == dev && ext2_sb_info[i].pt.p == p) {
			e = &ext2_sb_info[i];
			break;
		}
	}
	
	return e;
}
