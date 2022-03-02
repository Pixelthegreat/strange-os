/* kernel panic system */
#include "panic.h"
#include "kprint.h"

extern unsigned int apts[]; /* alloced page tables */

/* panic */
extern void kpanic(int e, char *s) {

	/* print panic message */
	kprint("[kernel panic] code ");
	kprinthex(e);
	kprint(": ");
	kprint(s);
	
	/* try to halt */
	while (1) { asm("hlt"); }
}
