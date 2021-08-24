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

/* entry point function -- called by kernel loader */
int kmain() {
	
	/* initialise gdt */
	init_gdt();
	
	/* load idt */
	isr_install();
	
	/* clear the screen buffer */
	kcls();
	
	/* hello world */
	char *s = "hello, world!\n";
	write(1, s, strlen(s));

	/* get scancodes */
	while (1) {
	
		/* wait for key press and receive scancode */
		u8 sc = kgetsc();
		
		/* print scancode as character */
		kprintc(sc);
	}
	
	return 0; /* exit */
}
