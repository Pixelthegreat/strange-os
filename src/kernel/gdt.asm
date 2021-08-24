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
gdt_end:
gdt_descriptor:
	gdt_size dw gdt_end - gdt_nulldesc - 1
	dd gdt_nulldesc
	
codeseg equ gdt_codeseg - gdt_nulldesc
dataseg equ gdt_dataseg - gdt_nulldesc

%endif
