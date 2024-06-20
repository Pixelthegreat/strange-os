OUTPUT_FORMAT(binary)
ENTRY(_start)
SECTIONS {
	. = 0x800000;
	.header : { *(.header) }
	.text : { *(.text) }
	.data : { *(.data) }
	.rodata : { *(.rodata) }
	.bss : { *(.bss) }
}
