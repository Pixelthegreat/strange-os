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

/* disk sector buffer */
char buffer[512];

char *s; /* string for storing boot messages */

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
	s = "[kernel] initializing paging...\n";
	write(1, s, strlen(s));
	paging_enable();

	/* initialize heap */
	s = "[kernel] initializing heap...\n";
	write(1, s, strlen(s));
	heap_init();

	/* intialize ata */
	s = "[kernel] initializing ata port...\n";
	write(1, s, strlen(s));
	ata_init();

	/* mount the vfs */
	s = "[kernel] mounting vfs...\n";
	write(1, s, strlen(s));
	fs_init();

	ext2_check_sig(0);

	/* search for a root device and partition */
	//s = "[kernel] searching for root device...\n";
	//write(1, s, strlen(s));
	//rp = dev_init_root();

	/* print info for device */
	//kprint("[kernel] root device is ");
	//kprinthex(rp.dev);
	//kprintnl();

	return 0; /* exit */
}
