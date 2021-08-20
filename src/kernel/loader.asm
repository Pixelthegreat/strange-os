; load the kernel ;
global loader
extern kernel_main
MAGIC equ 0x1badb002
FLAGS equ 0x3
CHECKSUM equ -(MAGIC+FLAGS)

section .text
align 4
; 'magic' header information ;
dd MAGIC
dd FLAGS
dd CHECKSUM

; actual loader ;
loader:
	; external reference ;D ;
	[extern kmain]

	call kmain
	jmp $
