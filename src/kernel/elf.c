#include "../types.h"
#include "../include/string.h"
#include "kprint.h"
#include "heap.h"
#include "fileio.h"
#include "process.h"
#include "elf.h"

/* load file to process memory */
extern int elf_load_file(const char *p, struct proc *pr) {

	kfd_t fd = kopen(p, "r");
	if (fd < 0) return -1;

	/* read file to buffer */
	struct fs_node *n = kgetnode(fd);
	void *buf = kmalloc(n->len);
	kread(fd, buf, n->len);
	kclose(fd);

	/* read header */
	struct elf32_header *hdr = (struct elf32_header *)buf;
	if (elf_check_header(hdr) < 0) {

		kfree(buf);
		return -1;
	}

	/* get first section header */
	struct elf32_section_header *shdr = (struct elf32_section_header *)(buf + hdr->shoff);
	if (shdr->type != ELF_SECTION_TYPE_NULL) {

		kfree(buf);
		return -1;
	}

	/* get string header */
	struct elf32_section_header *strtab = elf_get_string_table(hdr);
	if (strtab == NULL) {

		kfree(buf);
		return -1;
	}

	/* list other headers */
	for (int i = 1; i < hdr->shnum; i++) {

		struct elf32_section_header *sh = shdr + i;

		/* ignore */
		if (sh->addr < 0x800000 || !sh->size) continue;

		/* program data */
		void *addr = (pr->mem + (sh->addr - 0x800000));
		if (sh->type == ELF_SECTION_TYPE_PROGBITS)
			memcpy(addr, buf + sh->off, sh->size);

		/* bss usually */
		else if (sh->type == ELF_SECTION_TYPE_NOBITS)
			memset(addr, 0, sh->size);
	}

	/* set entry point */
	pr->ent = (void *)hdr->ent;

	/* ... free buffer */
	kfree(buf);
	return 0;
}

/* check header for correct values */
extern int elf_check_header(struct elf32_header *hdr) {

	/* check magic values */
	if (hdr->ident[EI_MAG0] != ELF_MAG0) return ELF_ERROR_SIG;
	else if (hdr->ident[EI_MAG1] != ELF_MAG1) return ELF_ERROR_SIG;
	else if (hdr->ident[EI_MAG2] != ELF_MAG2) return ELF_ERROR_SIG;
	else if (hdr->ident[EI_MAG3] != ELF_MAG3) return ELF_ERROR_SIG;

	/* check support */
	if (hdr->ident[EI_CLASS] != ELF_CLASS_32) return ELF_ERROR_CLASS;
	else if (hdr->ident[EI_DATA] != ELF_DATA_2LSB) return ELF_ERROR_DATA;
	else if (hdr->mach != ELF_MACHINE_386) return ELF_ERROR_TARGET;
	else if (hdr->ident[EI_VERSION] != ELF_VERSION_CURRENT) return ELF_ERROR_VER;
	else if (hdr->type != ELF_TYPE_EXEC) return ELF_ERROR_TYPE;

	/* no error */
	return 0;
}
