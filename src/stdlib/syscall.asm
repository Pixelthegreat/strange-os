	[bits 32]
	[global _syscall3]

section .text
_syscall3:
	push ebp
	mov ebp, esp
	sub esp, 0x10
	
	; move arguments ;
	mov eax, [ebp+0x8]
	mov ebx, [ebp+0xc]
	mov ecx, [ebp+0x10]
	mov edx, [ebp+0x14]
	int 0x40
	
	leave
	ret
