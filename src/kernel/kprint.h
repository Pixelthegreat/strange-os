#ifndef KPRINT_H
#define KPRINT_H

#include "../types.h"
#include "idt.h"

/* macros */
#define SCANCODE_BACKSPACE (u8)(0x0e)
#define SCANCODE_SHIFT_DOWN (u8)(0x2a)
#define SCANCODE_SHIFT_UP (u8)(0xaa)

#define KPRINT_WIDTH 40
#define KPRINT_HEIGHT 25

/* functions */
extern void ktextinit(void); /* initialize text engine */
extern void kcls(void); /* clear screen */
extern void kprint(const char *s); /* print string */
extern void kprintc(char c); /* print character */
extern void kprinthex(u32 h); /* print hexadecimal number */
extern void kprintnl(void); /* print newline */
extern void kscroll(void); /* update scrolling */
extern void kflush(void); /* flush output */

extern void kbint(registers *regs); /* keyboard interrupt */
extern u8 kgetsc(void); /* get scancode */
extern char kgetc(void); /* get character */
extern void kgets(char *buf, size_t sz); /* get string */

#endif /* KPRINT_H */
