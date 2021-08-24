; load the kernel ;
global loader
;extern kernel_main
;MAGIC equ 0x1badb002
;FLAGS equ 0x3
;CHECKSUM equ -(MAGIC+FLAGS)

section .text
;align 4
; 'magic' header information ;
;dd MAGIC
;dd FLAGS
;dd CHECKSUM

; idt stuff ;
%include "idt.asm"
%include "gdt.asm"

; actual loader ;
loader:
	; external reference ;D ;
	[extern kmain]
	
	;lgdt [gdt_descriptor]
	;jmp codeseg:loader_start

loader_start:
	
	cli ; clear interrupts ;
	call kmain
	jmp $
