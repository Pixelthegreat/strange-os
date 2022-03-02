; external references ;
extern irq_handler
extern isr_handler
extern int_handler

; make all isrs visible ;
global isr0
global isr1
global isr2
global isr3
global isr4
global isr5
global isr6
global isr7
global isr8
global isr9
global isr10
global isr11
global isr12
global isr13
global isr14
global isr15
global isr16
global isr17
global isr18
global isr19
global isr20
global isr21
global isr22
global isr23
global isr24
global isr25
global isr26
global isr27
global isr28
global isr29
global isr30
global isr31

; irqs ;
global irq0
global irq1
global irq2
global irq3
global irq4
global irq5
global irq6
global irq7
global irq8
global irq9
global irq10
global irq11
global irq12
global irq13
global irq14
global irq15

global irqsyscall

; actual isrs ;
isr0:
	push byte 0
	push byte 0
	jmp isr_common_stub
	
isr1:
	push byte 0
	push byte 1
	jmp isr_common_stub
	
isr2:
	push byte 0
	push byte 2
	
isr3:
	push byte 0
	push byte 3
	
isr4:
	push byte 0
	push byte 4
	
isr5:
	push byte 0
	push byte 5
	
isr6:
	push byte 0
	push byte 6
	
isr7:
	push byte 0
	push byte 7
	
isr8:
	push byte 0
	push byte 8
	
isr9:
	push byte 0
	push byte 9
	
isr10:
	push byte 0
	push byte 10
	
isr11:
	push byte 0
	push byte 11
	
isr12:
	; error information pushed by CPU ;
	push byte 12
	
isr13:
	push byte 0
	push byte 13
	
isr14:
	; error info pushed by CPU ;
	push byte 14
	
isr15:
	push byte 0
	push byte 15
	
isr16:
	push byte 0
	push byte 16
	
isr17:
	push byte 0
	push byte 17
	
isr18:
	push byte 0
	push byte 18
	
isr19:
	push byte 0
	push byte 19
	
isr20:
	push byte 0
	push byte 20
	
isr21:
	push byte 0
	push byte 21
	
isr22:
	push byte 0
	push byte 22
	
isr23:
	push byte 0
	push byte 13
	
isr24:
	push byte 0
	push byte 24
	
isr25:
	push byte 0
	push byte 25
	
isr26:
	push byte 0
	push byte 26
	
isr27:
	push byte 0
	push byte 27
	
isr28:
	push byte 0
	push byte 28
	
isr29:
	push byte 0
	push byte 29
	
isr30:
	push byte 0
	push byte 30
	
isr31:
	push byte 0
	push byte 31
	jmp isr_common_stub
	
; irqs ;
irq0:
	push byte 0
	push byte 32
	jmp irq_common_stub
	
irq1:
	push byte 0
	push byte 33
	jmp irq_common_stub
	
irq2:
	push byte 0
	push byte 34
	jmp irq_common_stub
	
irq3:
	push byte 0
	push byte 35
	jmp irq_common_stub
	
irq4:
	push byte 0
	push byte 36
	jmp irq_common_stub
	
irq5:
	push byte 0
	push byte 37
	jmp irq_common_stub
	
irq6:
	push byte 0
	push byte 38
	jmp irq_common_stub
	
irq7:
	push byte 0
	push byte 39
	jmp irq_common_stub
	
irq8:
	push byte 0
	push byte 40
	jmp irq_common_stub
	
irq9:
	push byte 0
	push byte 41
	jmp irq_common_stub
	
irq10:
	push byte 0
	push byte 42
	jmp irq_common_stub
	
irq11:
	push byte 0
	push byte 43
	jmp irq_common_stub
	
irq12:
	push byte 0
	push byte 44
	jmp irq_common_stub
	
irq13:
	push byte 0
	push byte 45
	jmp irq_common_stub
	
irq14:
	push byte 0
	push byte 46
	jmp irq_common_stub
	
irq15:
	push byte 0
	push byte 47
	jmp irq_common_stub

; syscall handler ;
irqsyscall:
	push byte 0
	push byte 0x40
	jmp int_common_stub
	
isr_common_stub:
	pusha
	
	mov ax, ds
	push eax
	
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	push esp
	
	call isr_handler
	pop eax
	
	pop eax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	popa
	
	add esp, 8
	
	iretd
	
int_common_stub:
	pusha
	
	mov ax, ds
	push eax
	
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	push esp
	
	call int_handler
	pop eax
	
	pop eax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	popa
	
	add esp, 8
	iretd
	
irq_common_stub:
	pusha
	
	mov ax, ds
	push eax
	
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	push esp
	
	call irq_handler
	pop eax
	
	pop eax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	popa
	
	add esp, 8
	
	iretd
