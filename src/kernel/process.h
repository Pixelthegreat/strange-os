#ifndef PROCESS_H
#define PROCESS_H

#include "../types.h"
#include "idt.h"

#define PROCESS_WINDOW ((void *)0x800000)
#define PROCESS_WINDOW_SIZE 0x200000
#define PROCESS_MEM_BASE (PROCESS_WINDOW + PROCESS_WINDOW_SIZE)

typedef int kpid_t;

/* process info */
struct proc {
	kpid_t id; /* process id (usually position in process stack) */
	void *mem; /* process memory */
	size_t sz; /* size in bytes */
	struct proc *p; /* previous */
	struct proc *n; /* next */
	registers r; /* registers info */
	void *ent; /* entry point */
};

/* functions */
extern struct proc *process_alloc(size_t sz); /* allocate and set current process */
extern struct proc *process_get_current(void); /* get current process */
extern int process_load(const char *p, registers *r); /* load a process from a path */
extern void process_map(void); /* map current process */
extern void process_jump(void); /* jump into current process */
extern void process_pop(registers *r); /* free current process and move back */

#endif /* PROCESS_H */
