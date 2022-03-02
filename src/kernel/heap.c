#include "heap.h"
#include "../include/string.h"

/* first heap node */
static struct block_hdr *first = (struct block_hdr *)0x800000; /* start of heap memory */
static size_t dmem_size = 0x800000; /* size of dynamic memory */

/* initialise dynamic memory */
extern void heap_init(void) {

	/* set values of first node */
	first->sz = 1;
	first->p = NULL;
	first->n = NULL;
	first->used = 1;
	
	memset((void *)first, 0, dmem_size);
}

/* find new block */
extern struct block_hdr *heap_next_node(size_t sz) {

	/* start from first block */
	struct block_hdr *cur = first;
	struct block_hdr *blk = NULL;
	u32 pos = sizeof(struct block_hdr) + cur->sz;
	
	/* loop through blocks until we reach the end or we find something that works */
	while (pos < dmem_size && blk == NULL && cur != NULL) {

		/* unused block */
		if (!cur->used && (cur->sz >= sz || (cur->n != NULL && sz <= ((void *)(cur->n) - (void *)(cur) + sizeof(struct block_hdr))) || (cur->n == NULL && (pos + sizeof(struct block_hdr) + sz) < dmem_size))) {
			
			blk = cur;
		}
		
		/* no next node */
		else if (cur->n == NULL && (pos + sizeof(struct block_hdr) + sz) < dmem_size) {

			blk = (struct block_hdr *)(0x800000 + pos);
			cur->n = blk;
			blk->p = cur;
		}
		
		/* advance */
		cur = cur->n;
		if (cur != NULL) pos += sizeof(struct block_hdr) + cur->sz;
	}
	
	/* return pointer */
	return blk;
}

/* (attempt to) malloc memory */
extern void *kmalloc(size_t sz) {

	/* find block */
	struct block_hdr *blk = heap_next_node(sz);

	if (blk == NULL)
		return NULL;
	
	/* set values */
	blk->sz = sz;
	blk->used = 1;
	
	/* return pointer */
	return ((void *)blk) + sizeof(struct block_hdr);
}

/* free memory */
extern void kfree(void *p) {

	struct block_hdr *blk = p - sizeof(struct block_hdr);

	/* set to unused */
	blk->used = 0;
}
