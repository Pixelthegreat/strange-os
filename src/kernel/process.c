#include "../types.h"
#include "../include/string.h"
#include "kprint.h"
#include "heap.h"
#include "page.h"
#include "fileio.h"
#include "abin.h"
#include "process.h"

static struct proc *cur = NULL; /* current process info */

/* allocate and set current process */
extern struct proc *process_alloc(size_t sz) {

	struct proc *p = kmalloc(sizeof(struct proc));
	
	/* current available */
	if (cur != NULL) {

		p->id = cur->id + 1;
		p->mem = cur->mem + cur->sz;
		p->sz = sz;
		p->p = cur;
	}
	else {

		p->id = 0;
		p->mem = PROCESS_MEM_BASE;
		p->sz = sz;
		p->p = NULL;
	}
	p->n = NULL;
	p->ent = NULL;

	/* return */
	cur->n = p;
	cur = p;
	return p;
}

/* get current process */
extern struct proc *process_get_current(void) {

	return cur;
}

/* load a process from a path */
extern int process_load(const char *p, registers *r) {

	kfd_t fd = kopen(p, "r");
	if (fd < 0) return -1;

	/* allocate process */
	struct proc *pr = process_alloc(PROCESS_WINDOW_SIZE);

	/* load */
	if (abin_load_file(p, pr) < 0) return -1;

	/* copy registers */
	if (pr->p) memcpy(&pr->p->r, r, sizeof(registers));

	/* map process */
	process_map();
	return 0;
}

/* map current process */
extern void process_map(void) {

	paging_map(PROCESS_WINDOW, cur->mem, PROCESS_WINDOW_SIZE / 0x1000);
}

extern void flush_tss(void);
extern void jump_user(void *);

/* jump into current process */
extern void process_jump(void) {

	__asm__("jmp $8,$0x800000");
	//goto *cur->ent;
}

/* free current process and move back */
extern void process_pop(registers *r) {

	cur = cur->p;
	kfree(cur->n);
	cur->n = NULL;

	/* copy registers */
	if (cur != NULL) {

		u32 ebx = r->ebx;
		memcpy(r, &cur->r, sizeof(registers));
		r->eax = ebx; /* keep return value */
		process_map();
	}
}
