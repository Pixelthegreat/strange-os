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

/* disk sector buffer */
char buffer[512];

/* entry point function -- called by kernel loader */
int kmain() {
	
	/* initialise gdt */
	init_gdt();
	
	/* load idt */
	isr_install();
	
	/* clear the screen buffer */
	kcls();

	/* intialise ata */
	ata_init();

	/* read data */
	ata_chs_read(0, 0, 1, 0, buffer);

	/* print data in buffer */
	write(1, (void *)&buffer[510], 2);
	
	return 0; /* exit */
}
