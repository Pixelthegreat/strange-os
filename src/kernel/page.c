#include "page.h"
#include "panic.h"
#include "../types.h"
#include "../include/string.h"

static struct page pd[1024] __attribute__((aligned(4096)));
static struct page kpt[1024] __attribute__((aligned(4096)));
static struct page *ptt = (struct page *)0x400000;
static u8 frames[0x400000 / 8];
static u32 nframes = 0x400000;
static u8 enabled = 0;

void *heap = (void *)0x800000; /* start of heap */

static void page_set_frame(u32);
static void page_clr_frame(u32);
static int page_test_frame(u32);
static u32 page_first_frame(void);

struct pages kpst;

/* enable paging */
extern void paging_enable(void) {
	
	/* set entries to not present by default */
	for (int i = 0; i < 1024; i++) {
	
		pd[i].p = 1; /* not present */
		pd[i].user = 0; /* only accessable through kernel-mode */
		pd[i].rw = 1; /* readable and writable to */
		pd[i].frame = i + 1024;
		pd[i].unused = 0;
	}
	
	/* set page table table */
	for (int i = 0; i < nframes; i++) {
	
		//if (i >= 2048) ptt[i].p = 0;
		//else ptt[i].p = 1;
		ptt[i].p = 0;
		ptt[i].user = 0;
		ptt[i].rw = 1;
		ptt[i].frame = i; /* frame */
		ptt[i].unused = 0;
	}
	
	/* set kpt as an entry in the pd */
	//pd[0].p = 1;
	//pd[0].user = 0;
	//pd[0].rw = 1;
	//pd[0].frame = (((u32)kpt) >> 12) & 0xFFFFF;
	//pd[1].p = 1;
	//pd[1].user = 0;
	//pd[1].rw = 1;
	//pd[1].frame = 0x1000 + 1;
	kpst = page_malloc(4096, 0, 1);

	/* flush tlb */
	paging_update();

	/* memset stuff */
	memset((void *)frames, 0, 0x400000 / 8);
}

/* update tlb */
extern void paging_update(void) {

	/* disable paging flag in cr0 */
	if (enabled) asm("push %eax;mov %cr0,%eax;and $0x7FFFFFFF, %eax;mov %eax,%cr0;pop %eax");

	/* load page directory to cr3 */
	asm("push %%eax;mov %0, %%eax;mov %%eax, %%cr3;pop %%eax"::"d"(pd));
	
	/* enable paging flag in cr0 */
	asm("push %eax;mov %cr0,%eax;or $0x80000000, %eax;mov %eax,%cr0;pop %eax");
	enabled = 1;
}

/* set a frame */
static void page_set_frame(u32 x) {

	u32 f = (x >> 12) & 0xFFFFF;
	frames[f/8] |= (0x1 << (f%8));
}

/* clear a frame */
static void page_clr_frame(u32 x) {

	u32 f = (x >> 12) & 0xFFFFF;
	frames[f/8] &= ~(0x1 << (f%8));
}

/* test a frame to see if it is set */
static int page_test_frame(u32 x) {

	u32 f = (x >> 12) & 0xFFFFF;
	return (frames[f/8] & (0x1 << (f%8)));
}

/* find first frame */
static u32 page_first_frame(void) {
	
	u32 i, j;
	for (i = 0; i < nframes; i++) {
	
		for (j = 0; j < 8; j++) {
		
			u32 t = 0x1 << j;
			if (!(frames[i] & t))
				return i * 8 + j;
		}
	}
	
	/* could not find it */
	return 0;
}

/* allocate a single frame */
extern void page_alloc(struct page *p, int us, int rw) {
	
	/* get first frame */
	u32 idx = page_first_frame();
	if (idx == 0 && page_test_frame(0))
		kpanic(E_NOPAGES, "ran out of pages\n");
	
	/* set info */
	page_set_frame(idx * 0x1000);
	p->p = 1;
	p->rw = rw;
	p->user = us;
	p->frame = idx;

	if (!enabled) asm("invlpg %0"::"m"(p));
}

/* allocate multiple frames */
extern struct pages page_malloc(size_t n, int us, int rw) {

	/* get first available frame for later */
	u32 f, nf = page_first_frame();

	/* allocate multiple frames */
	for (size_t i = 0; i < n; i++) {

		/* allocate it */
		page_alloc((struct page *)(0x400000) + f, us, rw);

		/* get next frame */
		f = page_first_frame();
	}
	
	/* return pages struct */
	struct pages ps;
	ps.pn = n;
	ps.p = (struct page *)(0x400000 + (nf * sizeof(struct page)));
	
	return ps;
}

/* free frame */
extern void free_frame(struct page *p) {

	/* free frame */
	u32 f;
	if (!(f=p->frame))
		return;
	
	page_clr_frame(f);
	p->frame = 0;
}
