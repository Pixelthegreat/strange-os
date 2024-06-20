; header guard ;
%ifndef _GDT_ASM
%define _GDT_ASM

; global descriptor table ;
gdt_nulldesc:
	dd 0
	dd 0
gdt_codeseg:
	dw 0xFFFF
	dw 0x0000
	db 0x00
	db 10011010b
	db 11001111b
	db 0x00
gdt_dataseg:
	dw 0xFFFF
	dw 0x0000
	db 0x00
	db 10010010b
	db 11001111b
	db 0x00
gdt_user_codeseg:
	dw 0xFFFF
	dw 0x0000
	db 0x00
	db 11111010b
	db 11001111b
	db 0x00
gdt_user_dataseg:
	dw 0xFFFF
	dw 0x0000
	db 0x00
	db 11110010b
	db 11001111b
	db 0x00
gdt_tss:
	dw 0x0068
	dw 0xA000
	db 0x00
	db 10001001b
	db 00000000b
	db 0x00
gdt_end:
gdt_descriptor:
	gdt_size dw gdt_end - gdt_nulldesc - 1
	dd gdt_nulldesc
	
codeseg equ gdt_codeseg - gdt_nulldesc
dataseg equ gdt_dataseg - gdt_nulldesc
user_codeseg equ gdt_user_codeseg - gdt_nulldesc
user_dataseg equ gdt_user_dataseg - gdt_nulldesc

%endif
