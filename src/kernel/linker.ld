ENTRY(loader)
SECTIONS {
	. = 0x100000; /* starts at this point */
	.boot : { *(.multiboot) }
	.text : { *(.text) }
	.data : { *(.data) }
	.bss : { *(.bss) }
}
