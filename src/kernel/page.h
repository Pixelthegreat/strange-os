#ifndef PAGE_H
#define PAGE_H

#include "../types.h"

/* page entries */
#define PAGE_DIR_ENT(a) ((u32)(a) & 0xfffff000)
#define PAGE_TAB_ENT(a) ((u32)(a) & 0xfffff000)

#define PAGE_ENT_P 0x1
#define PAGE_ENT_RW 0x2
#define PAGE_ENT_US 0x4
#define PAGE_ENT_PWT 0x8
#define PAGE_ENT_PCD 0x10
#define PAGE_ENT_A 0x20
#define PAGE_ENT_D 0x40
#define PAGE_ENT_PS 0x80
#define PAGE_ENT_G 0x100
#define PAGE_ENT_PAT 0x1000

/* page table */
extern void paging_enable(void);
extern void paging_map(void *v, void *p, int n); /* map address */
extern void paging_invalidate(void); /* invalidate tlb */

#endif /* PAGE _H */
