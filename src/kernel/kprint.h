/* kprint -- module for the terminal printing functions */
#ifndef _KPRINT_H
#define _KPRINT_H

/* types */
#include "../types.h"
#include "idt.h" /* registers type */

/* macros */
#define VGA_WIDTH 80 /* width of the vga text mode screen */
#define VGA_HEIGHT 25 /* height of the vga text mode screen */
#define VGA_DEFAULT_COLOR 0x08 /* default vga color when clearing the screen */

/* functions */
extern int kprint(char *); /* print text (returns result) */
extern int kprintc(char); /* print character (returns result) */
extern int kcls(void); /* clear the terminal screen */
extern int kprintcs(char, u8); /* print a character with a color */
extern int kprints(char *, u8); /* print a string with a color */
extern int kprintnl(void); /* print new line character */
extern int kscroll(void); /* scroll up if necessary */
extern u8 kgetsc(void); /* get scancode */
extern void kbint(registers *); /* keyboard interrupt */

#endif /* _KPRINT_H */
