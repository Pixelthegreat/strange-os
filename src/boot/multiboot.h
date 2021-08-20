#ifndef _MULTIBOOT_H
#define _MULTIBOOT_H

#include "../types.h" /* custom types */

/* information for multiboot */
struct _multiboot {

	u32 flags;
	u32 lowmem;
	u32 highmem;
	u32 bootdev;
	u32 cmdline;
	u32 modscnt;
	u32 modsaddr;
	/* elf conf section */
	struct {
		u32 num;
		u32 size;
		u32 addr;
		u32 shndx;
	} elf_sec;

	u32 mmaplen;
	u32 mmapaddr;
	u32 driveslen;
	u32 drivesaddr;
	u32 conftable;
	u32 bootlname;
	u32 apmtable;
	u32 vbecontrolinf;
	u32 vbemode;
	u32 vbeinterface_seg;
	u32 vbeinterface_off;
	u32 vbeinterface_len;
};

#endif /* _MULTIBOOT_H */
