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
#include "fs/ext2.h"
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

	/* clear the screen buffer */
	kcls();
	
	/* initilize paging */
	kprint("[kernel] initializing paging...\n");
	paging_enable();

	/* initialize heap */
	kprint("[kernel] initializing heap...\n");
	heap_init();

	/* intialize ata */
	kprint("[kernel] initializing ata port...\n");
	ata_init();

	/* mount the vfs */
	kprint("[kernel] mounting vfs...\n");
	fs_init();

	/* mount ext2fs */
	int res = ext2_check_sig(0);
	struct fs_node ext2_root = {.name = "/", .inode = 2, .dev = 0, .part = 1};
	struct fs_node *n = ext2_finddir(&ext2_root, "lost+found");
	kprinthex((u32)n);
	if (n != NULL) {
		kprintc(' ');
		kprint(n->name);
		kprintc(' ');
		kprinthex(n->inode);
	}
	kprintnl();

	/* search for a root device and partition */
	//s = "[kernel] searching for root device...\n";
	//write(1, s, strlen(s));
	//rp = dev_init_root();

	/* print info for device */
	//kprint("[kernel] root device is ");
	//kprinthex(rp.dev);
	//kprintnl();

	/* test vga16 */
	//vga_init(320, 200, 256);
	
	//for (int y = 0; y < 200; y++)
	//	for (int x = 0; x < 320; x++)
	//		vga_addr[(y * 320) + x] = 0x3;

	return 0; /* exit */
}
