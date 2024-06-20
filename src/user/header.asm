	[bits 32]
	[extern _start]

section .header
	jmp _start
	db 0x00, "AB", 0x01
