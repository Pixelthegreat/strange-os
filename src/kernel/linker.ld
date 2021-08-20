ENTRY(loader)
SECTIONS {
	. = 0x100000; /* starts at this point */
	.text : { *(.text) }
	.data : { *(.data) }
	.bss : { *(.bss) }
}
