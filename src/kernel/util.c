/* header */
#include "util.h"
#include "../types.h" /* custom types */

/* port byte out */
extern void portbout(u16 port, u8 data) {
	__asm__("outb %%al, %%dx" : : "a" (data), "d" (port));
}; /* port byte out */

/* port byte in */
extern u8 portbin(u16 port) {
	u8 res;
	__asm__("inb %%dx, %%al" : "=a" (res) : "d" (port));
	return res; /* result */
}; /* port byte in */

extern void portwout(u16 port, u16 data) {
	__asm__("outw %%ax, %%dx" : : "a"(data), "d"(port));
}; /* port word out */

extern u16 portwin(u16 port) {
	u16 res;
	__asm__("inw %%dx, %%ax" : "=a" (res) : "d"(port));
	return res; /* result */
}; /* port word in */
