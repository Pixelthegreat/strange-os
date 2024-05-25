#include "fat.h"
#include "../../types.h"
#include "../disk/mbr.h"
#include "../kprint.h"
#include "../dev.h"
#include "../heap.h"
#include "fs.h"

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

static u32 cbufs = 0; /* sector number */
static int cbufn = 0; /* cbuf is set */
static void *cbuf = NULL; /* fat buffer */

static struct fat_vbr *vbr = NULL;

/* functions */
struct fs_info fat_fs_info = {
};

/* read cluster */
static void _read_cluster(u32 cn, void *buf) {

	dev_read_lba(fat_info.dev, ((cn - 2) * fat_info.vbr->spc) + fat_info.stc + fat_info.addr, fat_info.vbr->spc, buf);
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
				u32 cl = (dent->lc << 8) | dent->hc;
				fdent->inode = cl;
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
				nroot->inode = 0;
				nroot->dev = dev;
				nroot->part = i;
				nroot->fs = &fat_fs_info;

				fs_root->ptr = nroot;

				/* read the root directory */
				if (fat_info.fsz < 32) /*_read_root_dir16(nroot)*/;
				else _read_dir32(nroot, vbr32->rc);
			}
		}
	}
	return fatfs;
}
