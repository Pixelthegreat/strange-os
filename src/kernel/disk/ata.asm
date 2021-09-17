global _asm_ata_chs_read

_asm_ata_chs_read:
	pushfd
	push eax
	push ebx
	push ecx
	push edx
	push edi
	
	mov edx, 1f6h
	mov al, bh
	and al, 00001111b
	or al, 00010000b
	out dx, al
	
	mov edx, 1f2h
	mov al, ch
	out dx, al
	
	mov edx, 1f3h
	mov al, bl
	out dx, al
	
	mov edx, 1f4h
	mov eax, ebx
	mov cl, 16
	shr eax, cl
	out dx, al
	
	mov edx, 1f5h
	mov eax, ebx
	mov cl, 24
	shr eax, cl
	out dx, al
	
	mov edx, 1f7h
	mov al, 20h
	out dx, al
	
	;jmp .end
	
.continue:
	in al, dx
	test al, 8
	jz .continue
	
	mov eax, 256
	xor bx, bx
	mov bl, ch
	mul bx
	mov ecx, eax
	mov edx, 1f0h
	rep insw

.end:

	pop edi
	pop edx
	pop ecx
	pop ebx
	pop eax
	popfd
	ret
