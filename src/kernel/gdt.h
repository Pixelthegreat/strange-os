/* global descriptor table */
#ifndef _GDT_H
#define _GDT_H

#include "../types.h" /* custom types */

/* gdt register */
typedef struct _gdtreg {
	u16 limit; /* size of gdt */
	u32 base; /* pointer to gdt */
} __attribute__((packed)) gdtreg;

/* gdt descriptor */
typedef struct {
	u16 lim0_15;
	u16 base0_15;
	u8 base16_23;
	u8 access;
	u8 lim16_19:4;
	u8 other:4;
	u8 base24_31;
} __attribute__((packed)) gdt_desc;

/* macros */
#define GDTBASE 0x00000800
#define GDTSIZE 0xFF

/* functions */
extern void init_gdt_desc(u32, u32, u8, u8, gdt_desc *);
extern void init_gdt(void);

#endif /* _GDT_H */
