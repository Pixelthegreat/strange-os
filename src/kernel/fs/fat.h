/* fat 12/16/32 filesystem driver */
#ifndef FAT_H
#define FAT_H

#include "../../types.h"
#include "fs.h"

struct fat_vbr {
	u8 jmp[3]; /* eb 3c 90 */
	char oem[8]; /* oem label */
	u16 bps; /* bytes per sector */
	u8 spc; /* sectors per cluster */
	u16 rbs; /* reserved sectors for boot */
	u8 nfat; /* number of tables */
	u16 nrtent; /* number of root dir entries */
	u16 ns; /* number of logical sectors */
	u8 mdt; /* media descriptor type */
	u16 spf; /* sectors per fat */
	u16 spt; /* sectors per track */
	u16 nsd; /* number of heads/sides */
	u32 nhs; /* number of hidden sectors */
	u32 nls; /* large sector count */
} __attribute__((packed));

/* fat 12/16 vbr extension */
struct fat16_vbr {
	struct fat_vbr v;
	u8 drv; /* drive number */
	u8 rsvd; /* windows nt flags */
	u8 sig; /* signature */
	u32 vid; /* volume id */
	char vlab[11]; /* volume label */
	char slab[8]; /* file system label */
} __attribute__((packed));

/* fat 32 vbr extension */
struct fat32_vbr {
	struct fat_vbr v;
	u32 spf; /* sectors per fat */
	u16 flags; /* flags */
	u8 ver[2]; /* version number */
	u32 rc; /* root dir cluster */
	u16 fss; /* fs info sector */
	u16 bbs; /* backup boot sector */
	u8 rsvd[12]; /* reserved bytes */
	u8 drv; /* drive number */
	u8 rsvd2; /* windows nt flags */
	u8 sig; /* signature */
	u32 vid; /* volume id */
	char vlab[11]; /* volume label */
	char slab[8]; /* file system label */
} __attribute__((packed));

/* fs info struct */
struct fat32_fsinfo {
	u32 lsig; /* lead signature */
	u8 rsvd[480]; /* reserved bytes */
	u32 psig; /* other signature */
	u32 nfc; /* number of free clusters */
	u32 sc; /* search cluster */
	u8 rsvd2[12]; /* reserved bytes */
	u32 tsig; /* trail signature */
} __attribute__((packed));

/* normal directory entry */
struct fat32_dirent {
	char fn[11]; /* short file name */
	u8 attr; /* attributes */
	u8 rsvd; /* windows nt */
	u8 cths; /* creation time in hundredths of second */
	u16 ct; /* creation time */
	u16 cd; /* creation date */
	u16 lad; /* last access date */
	u16 hc; /* high bits of cluster */
	u16 lmt; /* last modification time */
	u16 lmd; /* last modification date */
	u16 lc; /* low bits of cluster */
	u32 sz; /* size of file in bytes */
} __attribute__((packed));

/* long name directory entry */
struct fat32_dirent_long {
	u8 ord; /* ... */
	char fn1[10]; /* two byte chars */
	u8 attr; /* attributes (always 0x0f for long fns) */
	u8 ent; /* TACO TRUCK ARMADILLO */
	u8 chk; /* checksum */
	char fn2[12]; /* next 6 chars */
	u16 z; /* always zero */
	char fn3[4]; /* final two chars */
} __attribute__((packed));

extern struct fs_info fat_fs_info; /* functions */

extern int fat_check_sig(int); /* check for filesystems */

#endif /* FAT_H */
