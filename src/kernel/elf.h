/* quite literally an elf header */
#ifndef ELF_H
#define ELF_H

#include "../types.h"
#include "process.h"

/* errors */
#define ELF_ERROR_SIG -1
#define ELF_ERROR_CLASS -2
#define ELF_ERROR_DATA -3
#define ELF_ERROR_TARGET -4
#define ELF_ERROR_VER -5
#define ELF_ERROR_TYPE -6

/* data types */
typedef u16 elf32_half_t;
typedef u32 elf32_off_t;
typedef u32 elf32_addr_t;
typedef u32 elf32_word_t;
typedef s32 elf32_sword_t;

#define ELF_NIDENT 16

/* elf header */
struct elf32_header {
	u8 ident[ELF_NIDENT];
	elf32_half_t type; /* type of file (i.e. exec, rel, ...) */
	elf32_half_t mach; /* machine target */
	elf32_word_t ver; /* elf version, should be 1 */
	elf32_addr_t ent; /* entry point offset */
	elf32_off_t phoff; /* program header offset */
	elf32_off_t shoff; /* section header offset */
	elf32_word_t flags; /* flag info */
	elf32_half_t ehsz; /* main header size */
	elf32_half_t phentsz; /* program header size */
	elf32_half_t phnum; /* number of program headers */
	elf32_half_t shentsz; /* section header size */
	elf32_half_t shnum; /* number of section headers */
	elf32_half_t shstrndx; /* section header string table */
} __attribute__((packed));

/* identifier info */
enum elf_ident {
	EI_MAG0 = 0, /* 0x7f */
	EI_MAG1 = 1, /*  E   */
	EI_MAG2 = 2, /*  L   */
	EI_MAG3 = 3, /*  F   */
	EI_CLASS = 4,
	EI_DATA = 5,
	EI_VERSION = 6,
	EI_OS_ABI = 7,
	EI_ABI_VERSION = 8,
	EI_PAD = 9,
};

#define ELF_MAG0 0x7f
#define ELF_MAG1 'E'
#define ELF_MAG2 'L'
#define ELF_MAG3 'F'

#define ELF_DATA_2LSB 1
#define ELF_CLASS_32 1

/* elf file type */
enum elf_type {
	ELF_TYPE_NONE = 0,
	ELF_TYPE_REL = 1,
	ELF_TYPE_EXEC = 2,
};

#define ELF_MACHINE_386 3
#define ELF_VERSION_CURRENT 1

/* section header */
struct elf32_section_header {
	elf32_word_t name; /* symbol index for name */
	elf32_word_t type; /* type of header */
	elf32_word_t flags; /* flag info */
	elf32_addr_t addr; /* address to be loaded */
	elf32_off_t off; /* offset into file */
	elf32_word_t size; /* section size */
	elf32_word_t link; /* ??? */
	elf32_word_t info; /* ??? */
	elf32_word_t addralgn; /* alignment of address */
	elf32_word_t entsz; /* size of entry */
} __attribute__((packed));

#define ELF_SH_UNDEF 0x00

/* section header types */
enum elf_section_type {
	ELF_SECTION_TYPE_NULL = 0, /* nothing */
	ELF_SECTION_TYPE_PROGBITS = 1, /* program info */
	ELF_SECTION_TYPE_SYMTAB = 2, /* symbol table */
	ELF_SECTION_TYPE_STRTAB = 3, /* string table */
	ELF_SECTION_TYPE_RELA = 4, /* relocation */
	ELF_SECTION_TYPE_NOBITS = 8, /* not present */
	ELF_SECTION_TYPE_REL = 9, /* relocation */
};

/* section header attributes */
enum elf_section_attr {
	ELF_SECTION_ATTR_WRITE = 0x01, /* writable */
	ELF_SECTION_ATTR_ALLOC = 0x02, /* needs to be allocated */
};

/* functions */
extern int elf_load_file(const char *p, struct proc *pr); /* load a file to process memory */
extern int elf_check_header(struct elf32_header *hdr); /* check for correct header values */

/* get section for string table */
static inline struct elf32_section_header *elf_get_string_table(struct elf32_header *hdr) {

	if (hdr->shstrndx == ELF_SH_UNDEF) return NULL;
	return (struct elf32_section_header *)((void *)hdr + hdr->shoff) + hdr->shstrndx;
}

/* get string */
static inline char *elf_get_string(struct elf32_header *hdr, elf32_word_t off) {

	struct elf32_section_header *strtab = elf_get_string_table(hdr);
	return (char *)hdr + strtab->off + off;
}

#endif /* ELF_H */
