#include "gdt.h" /* global descriptor table */
#include "../include/string.h" /* string functions */

extern void init_gdt_desc(u32 base, u32 limit, u8 access, u8 other, gdt_desc *desc) {

	/* set values */
	desc->lim0_15 = (limit & 0xffff);
	desc->base0_15 = (base & 0xffff);
	desc->base16_23 = (base & 0xff0000) >> 16;
	desc->access = access;
	desc->lim16_19 = (limit & 0xf0000) >> 16;
	desc->other = (other & 0xf);
	desc->base24_31 = (base & 0xff000000) >> 24;
}

/* global descriptor table */
gdtreg kgdtr; /* gdt register */
gdt_desc kgdt[GDTSIZE]; /* gdt */

/* initialise gdt */
extern void init_gdt(void) {

	init_gdt_desc(0x0, 0x0, 0x0, 0x0, &kgdt[0]);
	init_gdt_desc(0x0, 0xFFFFF, 0x9B, 0x0D, &kgdt[1]);    /* code */
	init_gdt_desc(0x0, 0xFFFFF, 0x93, 0x0D, &kgdt[2]);    /* data */
	init_gdt_desc(0x0, 0x0, 0x97, 0x0D, &kgdt[3]);        /* stack */
	
	init_gdt_desc(0x0, 0xFFFFF, 0xFF, 0x0D, &kgdt[4]);    /* ucode */
	init_gdt_desc(0x0, 0xFFFFF, 0xF3, 0x0D, &kgdt[5]);    /* udata */
	init_gdt_desc(0x0, 0x0, 0xF7, 0x0D, &kgdt[6]);        /* ustack */

	kgdtr.limit = GDTSIZE * 8;
	kgdtr.base = GDTBASE;
	
	/* copy memory */
	memcpy((void *)kgdtr.base, (void *)kgdt, kgdtr.limit);

	/* load gdt */
	asm volatile("lgdtl (kgdtr)");

	/* initilise the segments */
	asm("   movw $0x10, %ax    \n \
		movw %ax, %ds    \n \
		movw %ax, %es    \n \
		movw %ax, %fs    \n \
		movw %ax, %gs    \n \
		ljmp $0x08, $next    \n \
		next:        \n");
}
