/* mbr system */
#ifndef _MBR_H
#define _MBR_H

#include "../../types.h"

/* macros */
#define MBR_TYPE_LINUX 0x83
#define MBR_TYPE_FAT16_A 0x04
#define MBR_TYPE_FAT16_B 0x06
#define MBR_TYPE_FAT16_C 0x0e
#define MBR_TYPE_FAT32_A 0x0b
#define MBR_TYPE_FAT32_B 0x0c

#define MBR_TYPE_IS_FAT(t) ((t) == MBR_TYPE_FAT16_A || (t) == MBR_TYPE_FAT16_B || (t) == MBR_TYPE_FAT16_C || (t) == MBR_TYPE_FAT32_A || (t) == MBR_TYPE_FAT32_B)

#define MBR_TYPE_IS_FAT16(t) ((t) == MBR_TYPE_FAT16_A || (t) == MBR_TYPE_FAT16_B || (t) == MBR_TYPE_FAT16_C)

#define MBR_TYPE_IS_FAT32(t) ((t) == MBR_TYPE_FAT32_A || (t) == MBR_TYPE_FAT32_B)

#define MBR_BOOTABLE (1 << 7)

/* mbr partition entry */
struct mbr_part_entry {
	u8 attrs; /* drive attributes (bootable, etc) */
	u8 chs_addr[3]; /* address of partition start in CHS values */
	u8 p_type; /* partition type */
	u8 chs_ls_addr[3]; /* address of partition end in CHS values */
	u32 lba_addr; /* address of partition start in LBA values */
	u32 nsects; /* number of sectors in partition */
};

/* functions */
extern struct mbr_part_entry mbr_get_part_entry(int n); /* get partition information */
extern struct mbr_part_entry mbr_get_part_entry_buf(void *buf, int n); /* get part info from a bootsector buffer */

/* convert logical sector number to chs values, vice versa */
static inline u32 l2chs(u32 l) {

	u16 c = (u16)(l / (63 * 16));
	u8  h = l % 16;
	u8  s = l % 63;
	return (((c << 16) | h) << 8) | s;
}
static inline u32 chs2l(u16 c, u8 h, u8 s) {

	return (c * 16 + h) * 63 + s;
}

#endif /* _MBR_H */
