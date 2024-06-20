#include "../types.h"
#include "../include/string.h"
#include "kprint.h"
#include "heap.h"
#include "fileio.h"
#include "process.h"
#include "abin.h"

/* load file to process memory */
extern int abin_load_file(const char *p, struct proc *pr) {

	kfd_t fd = kopen(p, "r");
	if (fd < 0) return -1;

	/* read header to buffer */
	struct abin_header hdr;
	kread(fd, &hdr, sizeof(hdr));

	/* check signature */
	if (hdr.sig[0] != 0x00 || hdr.sig[1] != 'A' || hdr.sig[2] != 'B') {

		kclose(fd);
		return -1;
	}

	/* copy header to memory */
	memcpy(pr->mem, &hdr, sizeof(hdr));

	/* read remainder of file */
	struct fs_node *n = kgetnode(fd);
	kread(fd, pr->mem + sizeof(hdr), n->len - sizeof(hdr));
	kclose(fd);

	/* set entry point */
	if (!hdr.ent) pr->ent = (void *)0x800000;
	else if (hdr.ent < 0x800000) pr->ent = (void *)(0x800000 + hdr.ent);
	else pr->ent = (void *)hdr.ent;

	return 0;
}
