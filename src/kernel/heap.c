#include "../types.h"
#include "heap.h"

static struct heap_block_hdr *head = (void *)0x100000;

/* initialize heap */
extern void heap_init(void) {

	head->sz = 0;
	head->av = 0;
	head->p = NULL;
	head->n = head + 1;

	/* next */
	head->n->sz = 0;
	head->n->av = 1;
	head->n->p = head;
	head->n->n = NULL;
}

/* find unused block */
extern struct heap_block_hdr *heap_find(struct heap_block_hdr *b, size_t sz) {

	struct heap_block_hdr *p = b;
	while (p != NULL) {

		/* usable */
		if ((p->sz >= sz || p->n == NULL) && p->av)
			return p;

		p = p->n;
	}
	return NULL;
}

/* allocate */
extern void *kmalloc(size_t sz) {

	struct heap_block_hdr *b = heap_find(head, sz);
	if (b == NULL) return NULL;

	/* set base values */
	b->sz = sz;
	b->av = 0;

	/* set values for next */
	if (b->n == NULL) {
		
		b->n = (void *)(b + 1) + sz;
		b->n->sz = 0;
		b->n->av = 1;
		b->n->p = b;
		b->n->n = NULL;
	}

	/* return data pointer */
	return (void *)(b + 1);
}

/* free */
extern void kfree(void *p) {

	struct heap_block_hdr *b = (struct heap_block_hdr *)p - 1;

	/* set values */
	b->av = 1;
}
