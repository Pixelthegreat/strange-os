	[bits 32]
	[global _start]
	[global _std_argv]
	[extern main]

section .text
_start:
	mov dword[_std_argv], ecx
	call main
	push eax ; save result ;
	
	mov eax, 0x6 ; exit syscall ;
	pop ebx ; use result ;
	mov ecx, 0
	mov edx, 0
	int 0x40
	
	jmp $ ; failed ;

section .bss
_std_argv:
resb 4
