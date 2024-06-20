/* include */
#include "idt.h"
#include "kprint.h"
#include "../include/string.h"
#include "../types.h"
#include "disk/ata.h"
#include "kprint.h"
#include "util.h"
#include "page.h"
#include "panic.h"
#include "fs/fat.h"
#include "dev.h"
#include "heap.h"
#include "video/vga16.h"
#include "fileio.h"
#include "process.h"
#include "abin.h"

//#define KDEBUG

extern void flush_tss(void);
extern void jump_user(void);

static char buffer[512];

static struct dev_part rp; /* root partition stuff */

/* update stack pointer in tss */
extern void kernel_set_stack(u32 addr) {

	*(u32 *)0xa004 = addr; /* esp0 */
	flush_tss();
}

/* entry point function -- called by kernel loader */
int kmain(void) {
	
	/* load idt */
	isr_install();

	/* initialize heap */
	heap_init();

	/* initialize vga driver */
	vga_init(320, 200, 256);
	vga_update();

	/* initialize text */
	ktextinit();
	
	/* initilize paging */
	kprint("[kernel] initializing paging...\n");
	paging_enable();

	/* intialize ata */
	kprint("[kernel] initializing ata port...\n");
	ata_init();

	/* mount the vfs */
	kprint("[kernel] mounting vfs...\n");
	fs_init();

	/* initialize file io */
	kprint("[kernel] initializing file io...\n");
	kfdinit();

	/* search for a root device and partition */
	kprint("[kernel] searching for root device...\n");
	rp = dev_init_root();

	/* print info for device */
	kprint("[kernel] root device is ");
	kprinthex(rp.dev);
	kprintnl();

	/* mount filesystems */
	kprint("[kernel] searching for filesystems...\n");
	if (fat_check_sig(rp.dev)) kprint("[kernel] root filesystem is fat\n");

	/* search for kernel */
	struct fs_node *kernel = fs_finddir_path(fs_root, "boot/strange");
	if (kernel != NULL) kprint("[kernel] found kernel\n");

	/* allocate process */
	struct proc *p = process_alloc(PROCESS_WINDOW_SIZE);

	/* load elf */
	if (abin_load_file("/bin/init", p) < 0) {

		kpanic(E_NOFS, "failed to load init\n");
	}
	else kprint("[kernel] sucessfully loaded init\n");

	/* jump to process */
	process_map();
	flush_tss();
	process_jump();

	/* exit */
	return 0;
}
