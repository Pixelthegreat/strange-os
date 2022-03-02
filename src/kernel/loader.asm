; load the kernel ;
global loader
extern kernel_main
;MAGIC equ 0x1badb002
;FLAGS equ 0x3
;CHECKSUM equ -(MAGIC+FLAGS)

section .text
;align 4
; 'magic' header information ;
;dd MAGIC
;dd FLAGS
;dd CHECKSUM
	jmp loader

%include "idt.asm" ; interrupt descriptor table ;
%include "gdt.asm" ; global descriptor table ;
%include "disk/ata.asm" ; ATA functions in low-level assembly ;

; actual loader ;
loader:
	; external reference ;D ;
	[extern kmain]
	[extern kprint]

	lgdt [gdt_descriptor]
	;mov ax, dataseg
	;mov ds, ax
	;mov es, ax
	;mov fs, ax
	;mov gs, ax
	jmp codeseg:loader_start
	;jmp loader_start

loader_start:

	cli ; clear interrupts ;
	call kmain

.end:
	; jump into infinite loop ;
	jmp $
	
section .data
	msg db "hello, world!", 0
section .bss
	buffer resb 512 ; temporary ata reading buffer ;
