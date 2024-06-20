/* simple binary format for testing */
#ifndef ABIN_H
#define ABIN_H

#include "../types.h"
#include "process.h"

struct abin_header {
	u8 jmp[1]; /* jmp code */
	u32 ent; /* entry point */
	u8 sig[3]; /* signature */
	u8 ver; /* version */
} __attribute__((packed));

/* functions */
extern int abin_load_file(const char *p, struct proc *pr);

#endif /* ABIN_H */
