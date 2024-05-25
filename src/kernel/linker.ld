OUTPUT_FORMAT(elf32-i386)
/* OUTPUT_FORMAT(binary) */
ENTRY(loader)
SECTIONS {
	. = 0x60000; /* starts at this point */
	.boot : { *(.multiboot) }
	.text : { *(.text) }
	.data : { *(.data) }
	.rodata : { *(.rodata) }
}
