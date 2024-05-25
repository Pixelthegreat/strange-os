#ifndef HEAP_H
#define HEAP_H

#include "../types.h"

struct heap_block_hdr {
	size_t sz; /* size of block */
	int av; /* available */
	struct heap_block_hdr *p; /* previous */
	struct heap_block_hdr *n; /* next */
};

/* functions */
extern void heap_init(void); /* initialize heap */
extern struct heap_block_hdr *heap_find(struct heap_block_hdr *b, size_t sz);

extern void *kmalloc(size_t sz);
extern void kfree(void *p);

#endif /* HEAP_H */
