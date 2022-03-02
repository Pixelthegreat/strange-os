[section .stage2]
[bits 16]

[extern a20_enable]
%include "../kernel/gdt.asm"

global stage2
stage2:
	cli
	call a20_enable
		
	lgdt [gdt_descriptor]
	mov eax, cr0
	or eax, 0x1
	mov cr0, eax
	jmp codeseg:entry32

[bits 32]
entry32:
	mov ax, dataseg
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	mov ebp, 0x80000
	mov esp, ebp
	
	jmp 0x10000

_halt:
	hlt
	jmp _halt
