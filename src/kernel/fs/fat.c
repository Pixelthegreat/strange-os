#include "fat.h"
#include "../../include/string.h"
#include "../../types.h"
#include "../disk/mbr.h"
#include "../kprint.h"
#include "../dev.h"
#include "../heap.h"
#include "fs.h"
#include "../panic.h"

static struct {
	kdev_t dev; /* device */
	size_t fsz; /* fat unit size */
	u32 addr; /* lba address */
	struct fat_vbr *vbr; /* vbr buffer */
	struct fat32_fsinfo *fsinfo; /* fsinfo struct */
	u32 spf; /* sectors per fat */
	u32 stf; /* start of fat */
	u32 stc; /* start of clusters */
} fat_info;

struct file_info {
	u32 cl; /* current cluster */
	u32 idx; /* index in cluster */
	void *buf; /* buffer pointer */
};

static u32 cbufs = 0; /* sector number */
static int cbufn = 0; /* cbuf is set */
static void *cbuf = NULL; /* fat buffer */

static struct fat_vbr *vbr = NULL;

/* functions */
struct fs_info fat_fs_info = {
	.r = fat_read,
	.w = fat_write,
	.o = fat_open,
	.c = fat_close,
	.fd = fat_filldir,
	.cr = fat_create,
};

/* read cluster */
static void _read_cluster(u32 cn, void *buf) {

	dev_read_lba(fat_info.dev, ((cn - 2) * fat_info.vbr->spc) + fat_info.stc + fat_info.addr, fat_info.vbr->spc, buf);
}

/* write cluster */
static void _write_cluster(u32 cn, void *buf) {

	dev_write_lba(fat_info.dev, ((cn - 2) * fat_info.vbr->spc) + fat_info.stc + fat_info.addr, fat_info.vbr->spc, buf);
}

/* read entry in fat on fat32 */
static u32 _read_fat_ent32(u32 cl) {

	u32 s = cl / 128;
	
	/* buffer not yet cached for sector */
	if (s != cbufs || !cbufn) {

		cbufs = s;
		cbufn = 1;
		dev_read_lba(fat_info.dev, fat_info.addr + fat_info.stf + s, 1, cbuf);
	}

	u32 f = cl % 128;
	return ((u32 *)cbuf)[f];
}

/* write entry in fat on fat32 */
static void _write_fat_ent32(u32 e, u32 cl) {

	u32 s = e / 128;

	/* buffer not yet cached for sector */
	if (s != cbufs || !cbufn) {

		cbufs = s;
		cbufn = 1;
		dev_read_lba(fat_info.dev, fat_info.addr + fat_info.stf + s, 1, cbuf);
	}

	u32 f = e % 128;
	((u32 *)cbuf)[f] = cl;

	/* write back */
	for (int i = 0; i < fat_info.vbr->nfat; i++) dev_write_lba(fat_info.dev, fat_info.addr + fat_info.stf + (fat_info.spf * i) + s, 1, cbuf);
}

/* find unused cluster */
static u32 _find_unused_cluster(void) {

	/* buffer not cached for starting sector */
	if (cbufs != 0 || !cbufn) {

		cbufs = 0;
		cbufn = 1;
		dev_read_lba(fat_info.dev, fat_info.addr + fat_info.stf, 1, cbuf);
	}

	/* loop for every sector or until unused cluster is found */
	for (int i = 0; i < fat_info.spf; i++) {

		for (int j = 0; j < 128; j++) {

			u32 cl = ((u32 *)cbuf)[j]; /* cluster number */
			u32 p = (i * 128) + j; /* cluster position */
			if (p < 2) continue; /* reserved */
			if (cl == 0) return p;
		}

		/* read next sector */
		if (i < fat_info.spf) {
			cbufs = i;
			dev_read_lba(fat_info.dev, fat_info.addr + fat_info.stf + i, 1, cbuf);
		}
	}
}

/* add directory entry */
static void _add_dirent(struct fs_node *node, char *fn, u32 type, struct dirent *d) {

	u32 clsz = fat_info.vbr->spc * 512; /* cluster size */
	
	/* read cluster */
	void *buf = kmalloc(clsz);
	u32 cl = node->inode;
	_read_cluster(cl, buf);

	/* until reach end */
	u32 i = 0; /* index into cluster */
	u32 pcl = cl; /* previous cluster */
	while (cl < 0x0FFFFFF8) {

		/* read next cluster */
		if (i >= clsz) {

			pcl = cl;
			cl = _read_fat_ent32(cl);
			i = 0;
			if (cl >= 0x0FFFFFF8) break;
			_read_cluster(cl, buf);
		}
		struct fat32_dirent *dent = (struct fat32_dirent *)(buf + i);

		/* stop */
		if ((i + 64) <= clsz && dent->fn[0] == 0 && (dent + 1)->fn[0] == 0) break;
		i += 32;
	}

	/* allocate new cluster */
	if (cl >= 0x0ffffff8) {

		u32 ncl = _find_unused_cluster();
		_write_fat_ent32(pcl, ncl);
		_write_fat_ent32(ncl, 0x0ffffff8);
		memset(buf, 0, clsz);
		cl = ncl;
	}

	/* get long and short dirents */
	struct fat32_dirent_long *ldent = (struct fat32_dirent_long *)(buf + i);
	struct fat32_dirent *dent = (struct fat32_dirent *)(buf + i + 32);

	/* copy short name */
	char *stext = strchr(fn, '.');
	int j; /* iter */
	if (stext++ != NULL) {

		for (j = 0; j < 3; j++) {
			if (stext[j] == 0) break;
			dent->fn[8 + j] = stext[j];
		}
		for (; j < 3; j++) {
			dent->fn[8 + j] = ' ';
		}
	}
	for (j = 0; j < 8; j++) {
		if (fn[j] == 0 || fn[j] == '.') break;
		dent->fn[j] = fn[j];
	}
	for (; j < 8; j++) dent->fn[j] = ' ';

	/* copy long name */
	char ntmp[26]; /* temporary name buffer */
	for (j = 0; j < 12; j++) {
		if (fn[j] == 0) break;
		ntmp[j * 2] = fn[j];
		ntmp[j * 2 + 1] = 0x00;
	}
	ntmp[j * 2] = 0x00; /* null terminator */
	ntmp[j * 2 + 1] = 0x00; /* same */

	/* padding */
	for (j++; j < 13; j++) {
		ntmp[j * 2] = 0xff;
		ntmp[j * 2 + 1] = 0xff;
	}

	for (j = 0; j < 26; j++) {
		if (j < 10) ldent->fn1[j] = ntmp[j];
		else if (j < 22) ldent->fn2[j-10] = ntmp[j];
		else ldent->fn3[j-22] = ntmp[j];
	}

	/* allocate first cluster */
	u32 fcl = _find_unused_cluster();
	_write_fat_ent32(fcl, 0x0ffffff8);

	/* other attributes */
	dent->attr = (type == FS_DIR)? FAT_DIRENT_DIRECTORY: 0;
	dent->rsvd = 0;
	dent->cths = 0;
	dent->ct = 0;
	dent->cd = 0;
	dent->lad = 0;
	dent->hc = (fcl >> 16) & 0xffff;
	dent->lmt = 0;
	dent->lmd = 0;
	dent->lc = fcl & 0xffff;
	dent->sz = 10;

	ldent->ord = 0x41;
	ldent->attr = FAT_DIRENT_LFN;
	ldent->ent = 0;
	ldent->chk = 0; /* to implement */
	ldent->z = 0;

	/* write cluster */
	_write_cluster(cl, buf);

	/* zero first cluster */
	memset(buf, 0, clsz);
	_write_cluster(fcl, buf);

	/* write main dirent info */
	d->node->impl2[0] = cl;
	d->node->impl2[1] = i;
	d->node->inode = fcl;
	d->inode = fcl;

	/* free buffer */
	kfree(buf);
}

/* read directory */
static void _read_dir32(struct fs_node *node, u32 cl) {

	struct dirent *fdent = NULL; /* current */

	struct fat32_vbr *vbr = (struct fat32_vbr *)fat_info.vbr;
	size_t clsz = fat_info.vbr->spc * 512; /* cluster size */

	/* get first cluster of root dir */
	void *buf = kmalloc(clsz);
	_read_cluster(cl, buf);

	/* until reach end */
	u32 i = 0; /* index into cluster */
	int fld = 0; /* set if follows long dir entry */
	while (cl < 0x0FFFFFF8) {

		/* read next cluster */
		if (i >= clsz) {

			cl = _read_fat_ent32(cl);
			i = 0;
			if (cl >= 0x0FFFFFF8) break;
			else _read_cluster(cl, buf);
		}
		struct fat32_dirent *dent = (struct fat32_dirent *)(buf + i);

		/* long */
		if (dent->attr == 0x0f) {

			struct fat32_dirent_long *ldent = (struct fat32_dirent_long *)dent;
			fld = 1;

			/* create dirent */
			struct dirent *d = fs_dirent_alloc();
			d->node = NULL;
			d->next = NULL;

			/* copy name */
			for (int i = 0; i < 5; i++) d->name[i] = ldent->fn1[i * 2];
			for (int i = 0; i < 6; i++) d->name[i+5] = ldent->fn2[i * 2];
			for (int i = 0; i < 2; i++) d->name[i+11] = ldent->fn3[i * 2];

			/* current */
			if (!fdent) {

				node->first = d;
				fdent = d;
			}
			else {

				fdent->next = d;
				fdent = d;
			}
		}

		/* normal */
		else {

			if (fld) {

				fld = 0;
				u32 pcl = ((u32)dent->hc << 16) | dent->lc;
				fdent->inode = pcl;
				fdent->node = fs_alloc(node);
				fdent->node->mask = (dent->attr & FAT_DIRENT_DIRECTORY)? FS_DIR: FS_FILE;
				fdent->node->inode = pcl;
				fdent->node->len = dent->sz;
				fdent->node->impl2[0] = cl;
				fdent->node->impl2[1] = i;
				strcpy(fdent->node->name, fdent->name);
			}
		}

		/* increment */
		i += 32;
	}

	/* free buffer */
	kfree(buf);
}

/* check for fat filesystem */
extern int fat_check_sig(kdev_t dev) {

	int fatfs = 0;

	/* load boot sector */
	char buf[512];
	dev_read_lba(dev, 0, 1, (void *)buf);

	/* loop through partitions */
	for (int i = 1; i < 5; i++) {

		struct mbr_part_entry pe;
		pe = mbr_get_part_entry_buf(buf, i);

		/* fat partition */
		if (MBR_TYPE_IS_FAT(pe.p_type)) {

			fatfs |= (1 << (i - 1));

			/* read first sector (todo: make better multiple fs management) */
			fat_info.dev = dev;
			fat_info.addr = pe.lba_addr;
			fat_info.vbr = (struct fat_vbr *)kmalloc(512);
			dev_read_lba(dev, pe.lba_addr, 1, fat_info.vbr);

			/* create cluster buffer */
			if (cbuf == NULL) cbuf = kmalloc(fat_info.vbr->bps * fat_info.vbr->spc);

			/* get type */
			if (MBR_TYPE_IS_FAT16(pe.p_type)) fat_info.fsz = 16;
			else if (MBR_TYPE_IS_FAT32(pe.p_type)) fat_info.fsz = 32;
			else fat_info.fsz = 12;

			struct fat16_vbr *vbr16 = (struct fat16_vbr *)fat_info.vbr;
			struct fat32_vbr *vbr32 = (struct fat32_vbr *)fat_info.vbr;

			/* get useful values */
			fat_info.stf = fat_info.vbr->rbs; /* start of file allocation table */
			if (fat_info.fsz < 32) {
			       	
				fat_info.spf = fat_info.vbr->spf;
				fat_info.stc = fat_info.stf + fat_info.vbr->nfat * fat_info.spf + (fat_info.vbr->nrtent * 32 / 512);
			}
			else {
				fat_info.spf = vbr32->spf;

				/* get fsinfo struct */
				if (vbr32->fss != 0) {

					fat_info.fsinfo = (struct fat32_fsinfo *)kmalloc(512);
					dev_read_lba(dev, pe.lba_addr + vbr32->fss, 1, (void *)fat_info.fsinfo);
				}
				fat_info.stc = fat_info.stf + fat_info.vbr->nfat * fat_info.spf;
			}

			/* create root dir node */
			if (!fs_root->ptr) {

				struct fs_node *nroot = fs_alloc(NULL);
				nroot->flags = FS_DIR;
				nroot->inode = vbr32->rc;
				nroot->dev = dev;
				nroot->part = i;
				nroot->fs = &fat_fs_info;

				fs_root->ptr = nroot;

				/* read the root directory */
				if (fat_info.fsz < 32) /*_read_root_dir16(nroot)*/;
				else _read_dir32(nroot, vbr32->rc);

				//struct dirent dummy;
				//_add_dirent(nroot, "random3.txt", FS_FILE, &dummy);
			}
		}
	}
	return fatfs;
}

/* read file */
extern u32 fat_read(struct fs_file *fp, u32 c, void *buf) {

	if (fp->m != FS_MODE_R) return 0;

	/* get info */
	struct file_info *inf = (struct file_info *)fp->d;
	size_t clsz = fat_info.vbr->spc * 512;

	u32 t; /* total bytes read */
	for (t = 0; t < c; t++) {
		if (fp->idx + t >= fp->n->len) break;

		/* read next cluster */
		if (inf->idx >= clsz) {

			u32 cl = _read_fat_ent32(inf->cl);
			if (cl >= 0x0ffffff8) break;
			_read_cluster(cl, inf->buf);
			inf->cl = cl;
			inf->idx = 0;
		}
		((u8 *)buf)[t] = ((u8 *)inf->buf)[inf->idx++]; /* set byte */
	}
	return t;
}

/* write callback */
extern u32 fat_write(struct fs_file *fp, u32 c, void *buf) {

	if (fp->m != FS_MODE_W) return 0;

	/* get info */
	struct file_info *inf = (struct file_info *)fp->d;
	size_t clsz = fat_info.vbr->spc * 512;

	u32 t; /* bytes written */
	for (int t = 0; t < c; t++) {

		/* write cluster */
		if (inf->idx >= clsz) {

			u32 cl = _read_fat_ent32(inf->cl);
			if (cl >= 0x0ffffff8) {
				cl = _find_unused_cluster();
				_write_fat_ent32(inf->cl, cl);
				_write_fat_ent32(cl, 0x0ffffff8);
			}

			/* write data */
			_write_cluster(inf->cl, inf->buf);
			inf->idx = 0;
			inf->cl = cl;
		}
		((u8 *)inf->buf)[inf->idx++] = ((u8 *)buf)[t];
	}
	return t;
}

/* open callback */
extern void fat_open(struct fs_file *fp) {

	fp->d = kmalloc(sizeof(struct file_info) + fat_info.spf * 512);
	struct file_info *inf = fp->d;
	inf->buf = inf + 1;
	inf->cl = fp->n->inode;
	if (fp->m == FS_MODE_R) _read_cluster(inf->cl, inf->buf);
	inf->idx = 0;
}

/* close callback */
extern void fat_close(struct fs_file *fp) {

	struct file_info *inf = (struct file_info *)fp->d;

	/* finish write operations */
	if (fp->m == FS_MODE_W) {

		if (inf->idx > 0) _write_cluster(inf->cl, inf->buf);

		/* reset cluster chain */
		u32 cl = inf->cl;
		u32 i = 0;
		while (1) {
			if (cl < 0x0ffffff8) break; /* idk why but putting the condition in the while loop causes fp->n to equal NULL prematurely */

			u32 pcl = _read_fat_ent32(cl);
			if (!i) _write_fat_ent32(cl, 0x0ffffff8);
			else _write_fat_ent32(cl, 0);
			cl = pcl;
			i++;
		}

		/* write file length */
		fp->n->len = fp->idx;
		
		void *buf = kmalloc(fat_info.vbr->spc * 512);
		_read_cluster(fp->n->impl2[0], buf);
		struct fat32_dirent *dent = (struct fat32_dirent *)(buf + fp->n->impl2[1]);
		dent->sz = fp->idx;
		_write_cluster(fp->n->impl2[0], buf);
		kfree(buf);
	}

	kfree(fp->d);
}

/* fill a directory */
extern void fat_filldir(struct fs_node *n) {

	/* function only called when dir needs to be filled */
	if (n->first == NULL && fat_info.fsz == 32) _read_dir32(n, n->inode);
}

/* create a file */
extern void fat_create(struct fs_node *n, char *fn, u32 tp, struct dirent *d) {

	_add_dirent(n, fn, tp, d);
}
