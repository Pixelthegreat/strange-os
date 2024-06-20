; file for loading the kernel, as well as some low level assembly functions ;
global loader
global flush_tss
global jump_user
extern kernel_main

section .text
	jmp loader

%include "src/kernel/idt.asm" ; interrupt descriptor table ;
%include "src/kernel/gdt.asm" ; global descriptor table ;
%include "src/kernel/disk/ata.asm" ; ATA functions in low-level assembly ;

; update tss info ;
flush_tss:
	pusha
	mov ax, 40
	ltr ax
	popa
	ret

addr: dd 0

; jump to user mode ;
jump_user:
	pop dword[addr]
	push dword[addr]
	
	mov ax, 32 ; data selector ;
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	;xor edx, edx
	;mov eax, 0x8
	;mov ecx, 0x174
	;wrmsr
	
	;mov edx, 0x800000
	;mov ecx, esp
	;sysexit
	mov eax, esp
	push 32
	push eax
	pushf
	push 24 ; code selector ;
	push dword[addr] ; location of process window ;
	
	iret

; actual loader ;
loader:
	[extern kmain]

	; setup tss ;
	mov dword[0xa004], 0x400000
	mov dword[0xa008], 8
	
	; load gdt
	lgdt [gdt_descriptor]
	mov ax, dataseg
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
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
