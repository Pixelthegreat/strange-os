/* header */
#include "util.h"
#include "../types.h" /* custom types */

/* port byte out */
extern void portbout(u16 port, u8 data) {
	__asm__("out %%al, %%dx" : : "a" (data), "d" (port));
}; /* port byte out */

/* port byte in */
extern u8 portbin(u16 port) {
	u8 res;
	__asm__("in %%dx, %%al" : "=a" (res) : "d" (port));
	return res; /* result */
}; /* port byte in */
