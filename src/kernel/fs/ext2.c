#include "ext2.h"
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
};

/* list of stuff */
static struct ext2_sb_info ext2_sb_info[8]; /* 8 ext fs's open at once i guess for the time being or whatever */
static int ext2_sb_info_n = 0;
static int ext2_sb_info_c = 8;

static char *randbuf = NULL; /* buffer for storing stuff */

/* get block group of inode */
#define EXT2_BLOCKGROUP(e, inode) (((inode) - 1) / (e)->sb->nipg)
#define EXT2_INDEX(e, inode) (((inode) - 1) % (e)->sb->nipg)
#define EXT2_CONT_BLOCK(e, idx) (((idx) * 128) / (e)->bs)

/* functions */
static int ext2_load_bgdt(struct ext2_sb_info *);
static int ext2_read_inode(struct ext2_sb_info *, int, struct ext2_inode *);

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
				
				/* malloc some sb stuff */
				struct ext2_sb *sb = (struct ext2_sb *)kmalloc(1024);
				memcpy(sb, sbb, 1024); /* copy superblock data */
				e->sb = sb;
				/* load block group descriptor table */
				ext2_load_bgdt(e);

				/* set value */
				ext2fs |= (1 << (i-1));
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
	
	/* read root directory inode */
	char *rootdir_buf = (char *)kmalloc(128);
	struct ext2_inode *rootdir = (struct ext2_inode *)rootdir_buf;
	
	ext2_read_inode(e, 2, rootdir);
	
	kprinthex(rootdir->tp);
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
