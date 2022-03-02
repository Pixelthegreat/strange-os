/* kernel heap memory */
#ifndef _HEAP_H
#define _HEAP_H

#include "../types.h" /* universal types */

/* heap block header */
struct block_hdr {
	size_t sz; /* number of bytes block uses */
	u8 used; /* node is being used */
	struct block_hdr *p; /* previous block header */
	struct block_hdr *n; /* next block header */
};

/* functions */
extern void heap_init(void); /* intialize heap */
extern struct block_hdr *heap_next_node(size_t); /* search for an available node */
extern void *kmalloc(size_t); /* allocate some heap memory */
extern void *krealloc(void *, size_t); /* reallocate some heap memory */
extern void kfree(void *); /* free some heap memory */

#endif /* _HEAP_H */
