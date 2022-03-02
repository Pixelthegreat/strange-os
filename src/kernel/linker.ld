OUTPUT_FORMAT(elf32-i386)
ENTRY(loader)
SECTIONS {
	. = 0x100000; /* starts at this point */
	.boot : { *(.multiboot) }
	.text : { *(.text) }
	.data : { *(.data) }
	.rodata : { *(.rodata) }
}
