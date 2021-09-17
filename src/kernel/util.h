/* utilities */
#ifndef _UTIL_H
#define _UTIL_H

/* custom types */
#include "../types.h"

/* functions */
extern void portbout(u16, u8);
extern u8 portbin(u16);
extern void portwout(u16, u16);
extern u16 portwin(u16);

/* macros */
#define insl(p, b, n) __asm__("cld;rep insl"::"d"(p), "D"(b), "c"(n))
#define outsl(p, b, n) __asm__("cld;rep outsl"::"d"(p), "S"(b), "c"(n))

/* wait for 1 to 4 microseconds */
static inline void io_wait(void) {

	portbout(0x80, 0);
}

#endif /* _UTIL_H */
