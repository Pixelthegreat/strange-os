#include "../types.h"
#include "../include/string.h"
#include "kprint.h"
#include "page.h"

static u32 p_dir[1024] __attribute__((aligned(4096))); /* page directory */
static u32 *p_tab = (u32 *)0x400000; /* arbitrary page tables location */

/* enable paging */
extern void paging_enable(void) {

	/* set first two page tables */
	p_dir[0] = PAGE_DIR_ENT(p_tab) | PAGE_ENT_P | PAGE_ENT_RW;
	p_dir[1] = PAGE_DIR_ENT(p_tab + 1024) | PAGE_ENT_P | PAGE_ENT_RW;

	/* identity remainder */
	for (int i = 2; i < 1024; i++)
		p_dir[i] = PAGE_DIR_ENT(p_tab + i * 1024) | PAGE_ENT_P | PAGE_ENT_RW | PAGE_ENT_US;
	
	/* set entries in page table */
	for (int i = 0; i < 2048; i++)
		p_tab[i] = PAGE_TAB_ENT(i * 0x1000) | PAGE_ENT_P | PAGE_ENT_RW;
	for (int i = 2048; i < 1024 * 1023; i++)
		p_tab[i] = PAGE_TAB_ENT(i * 0x1000) | PAGE_ENT_P | PAGE_ENT_RW | PAGE_ENT_US;

	/* set registers */
	__asm__("mov %0, %%cr3" : : "r" ((u32)p_dir));
	u32 cr0;
	__asm__("mov %%cr0, %0": "=r" (cr0));
	cr0 |= 0x80000000;
	__asm__("mov %0, %%cr0": : "r" (cr0));
}

/* set page table entry */
static void _set_pt_entry(u32 e, void *p) {

	p_tab[e] = PAGE_TAB_ENT(p) | PAGE_ENT_P | PAGE_ENT_RW | PAGE_ENT_US;
}

/* map address */
extern void paging_map(void *v, void *p, int n) {

	u32 ent = ((u32)v & 0xfffff000) >> 12;
	for (int i = 0; i < n; i++)
		_set_pt_entry(ent + i, p + i * 0x1000);
}

/* invalidate tlb */
extern void paging_invalidate(void) {

	__asm__("mov %cr3,%eax\nmov %eax,%cr3");
}
