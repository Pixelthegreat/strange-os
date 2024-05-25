#include "../types.h"
#include "../include/string.h"
#include "kprint.h"
#include "page.h"

static u32 p_dir[1024] __attribute__((aligned(4096))); /* page directory */
static u32 *p_tab = (u32 *)0x200000; /* arbitrary page tables location */

/* enable paging */
extern void paging_enable(void) {

	memset(p_dir, 0, 4096);

	/* set first page table */
	p_dir[0] = PAGE_DIR_ENT(p_tab) | PAGE_ENT_P | PAGE_ENT_RW;
	for (int i = 0; i < 1024; i++) {
		p_tab[i] = PAGE_TAB_ENT(i * 0x1000) | PAGE_ENT_P | PAGE_ENT_RW;
	}

	/* set registers */
	asm volatile("mov %0, %%cr3" : : "r" ((u32)p_dir));
	u32 cr0;
	asm volatile("mov %%cr0, %0": "=r" (cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0": : "r" (cr0));
}
