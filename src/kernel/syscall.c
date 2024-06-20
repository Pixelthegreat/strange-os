#include "syscall.h" /* system calls */
#include "kprint.h" /* terminal */
#include "../include/string.h" /* string functions */
#include "process.h"
#include "fileio.h"
#include "page.h"
#include "driver.h"

/* syscall interrupt handler */
extern void ksysint(registers *r) {
	
	__asm__("sti");
	
	/* print a message */
	switch (r->eax) {
		case 0x00: /* none */
			break;
		case 0x01: /* open */
			r->eax = (u32)kopen((const char *)r->ebx, (const char *)r->ecx);
			break;
		case 0x02: /* read */
			r->eax = (u32)kread((kfd_t)r->ebx, (void *)r->ecx, (size_t)r->edx);
			break;
		case 0x03: /* write */
			r->eax = (u32)kwrite((kfd_t)r->ebx, (void *)r->ecx, (size_t)r->edx);
			break;
		case 0x04: /* close */
			kclose((kfd_t)r->ebx);
			r->eax = 0;
			break;
		case 0x05: /* exec */
			int res = process_load((const char *)r->ebx, r);
			if (res < 0) {
				r->eax = (u32)res;
				break;
			}

			/* invalidate tlb */
			paging_invalidate();

			/* update registers */
			r->cs = 8;
			r->eip = (u32)PROCESS_WINDOW;
			struct proc *pr = process_get_current();
			r->ecx = r->ecx + ((u32)pr->p->mem - (u32)PROCESS_WINDOW);
			break;
		case 0x06: /* exit */
			process_pop(r);
			paging_invalidate();
			break;
		case 0x07: { /* readdir */
			struct fs_node *n;
			struct dirent *dent;
			r->eax = 0;
			if ((n = kgetnode(r->ebx)) == NULL) break;
			if ((dent = fs_readdir(n, r->ecx)) == NULL) break;
			r->eax = (u32)dent; /* to be implemented better */
			break;
		}
		case 0x08: { /* driverid */
			r->eax = (u32)driver_get_id((const char *)r->ebx);
			break;
		}
		case 0x09: { /* driverctl */
			r->eax = (u32)driver_control(r->ebx, r->ecx, r->edx);
			break;
		}
		default:
			kprint("unrecognized syscall number: ");
			kprinthex(r->eax);
			kprintnl();
			break;
	}
	__asm__("cli");
}
