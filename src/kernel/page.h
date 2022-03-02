/* page tables and stuff */
#ifndef _PAGE_H
#define _PAGE_H

#include "../types.h" /* types */

/* page */
struct page {
	union {
		struct {
			u32 p:1; /* if the page is present */
			u32 rw:1; /* if the page is writable */
			u32 user:1; /* if the page is in userland */
			u32 acc:1; /* if the page has been accessed */
			u32 d:1; /* ??? */
			u32 unused:7; /* unused */
			u32 frame:20; /* location of page */
		};
		u32 v; /* value in int form */
	};
};

/* multiple pages */
struct pages {
	struct page *p; /* first page */
	size_t       pn; /* number of pages */
};

/* enable paging */
extern void paging_enable(void);

/* allocate a page table */
//extern struct page_table page_alloc(int, int);
//extern void page_free(struct page_table);
extern void page_alloc(struct page *, int, int); /* allocate page */
extern struct pages page_malloc(size_t, int, int); /* allocate multiple pages */
extern void page_free(struct page *); /* free a page */
extern void page_mfree(struct pages);
extern void paging_update(void); /* update tlb */

#endif /* _PAGE_H */
