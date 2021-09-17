#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "idt.h" /* idt */

/* functions */
extern void ksysint(registers *); /* interrupt for syscalls */

#endif /* _SYSCALL_H */
