/* include */
#include "idt.h" /* interrupt descriptor table */
#include "gdt.h" /* global descriptor table */

/* utils */
#include "kprint.h"

/* stdlib */
#include "../include/unistd.h"
#include "../include/string.h"

/* types */
#include "../types.h"

/* kernel stuff */
#include "disk/ata.h"
#include "kprint.h"
#include "util.h"
#include "page.h"
#include "panic.h"
#include "fs/fat.h"
#include "dev.h"
#include "heap.h"
#include "video/vga16.h"

char buffer[512]; /* disk sector buffer */

struct dev_part rp; /* root partition stuff */

/* entry point function -- called by kernel loader */
int kmain(void) {
	
	/* initialise gdt */
	init_gdt();
	
	/* load idt */
	isr_install();

	/* initialize heap */
	heap_init();

	/* initialize vga driver */
	vga_init(320, 200, 256);
	vga_update();

	/* initialize text */
	ktextinit();
	
	/* initilize paging */
	kprint("[kernel] initializing paging...\n");
	paging_enable();

	/* intialize ata */
	kprint("[kernel] initializing ata port...\n");
	ata_init();

	/* mount the vfs */
	kprint("[kernel] mounting vfs...\n");
	fs_init();

	/* search for a root device and partition */
	kprint("[kernel] searching for root device...\n");
	rp = dev_init_root();

	/* print info for device */
	kprint("[kernel] root device is ");
	kprinthex(rp.dev);
	kprintnl();

	/* mount filesystems */
	kprint("[kernel] searching for filesystems on root device...\n");
	if (fat_check_sig(rp.dev)) kprint("[kernel] root filesystem is fat\n");

	/* demo */
	char buf[8];
	while (1) {
		kgets(buf, 8);
		kprint(buf);
		kprintnl();
	}

	/* exit */
	return 0;
}
