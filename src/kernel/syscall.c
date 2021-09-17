#include "syscall.h" /* system calls */
#include "kprint.h" /* terminal */
#include "../include/unistd.h" /* unistd io */
#include "../include/string.h" /* string functions */

/* syscall interrupt handler */
extern void ksysint(registers *r) {
	
	/* print a message */
	kprint("syscall\n");
}
