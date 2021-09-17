/* interrupt descriptor table */
#ifndef _IDT_H
#define _IDT_H

#include "../types.h" /* custom types */

/* macros */
#define low16(x) (u16)((x) & 0xFFFF)
#define high16(x) (u16)(((x) >> 16) & 0xFFFF)

/* idt gate type */
typedef struct {
	u16 low_offset; /* low offset of handler */
	u16 selector; /* more info */
	u8 always0; /* always set to zero */
	u8 flags; /* flag information */
	u16 high_offset; /* high offset */
} __attribute__((packed)) idt_gate;

/* register type */
typedef struct {
	u32 ds; /* data segment */
	u32 edi, esi, ebp, esp, ebx, edx, ecx, eax; /* gp registers */
	u32 int_no, err_code; /* pushed by isr proc */
	u32 eip, cs, eflags, useresp, ss; /* pushed automatically */
} __attribute__((packed)) registers;

/* idt register */
typedef struct {
	u16 limit; /* number of items */
	u32 base; /* pointer to first */
} __attribute__((packed)) idt_reg_t;

/* typedefs */
typedef void (*isr)(registers *); /* isr type */

/* functions */
extern void set_idt_gate(int, u32); /* set an idt gate to be a handler */
extern void isr_handler(registers *); /* handle an isr */
extern void isr_install(void); /* setup isrs */
extern void int_handler(registers *); /* handle any extra interrupts */
extern void irq_handler(registers *); /* handle an irq */
extern void isr_enable(void); /* enable isrs */
extern void idt_load(void); /* load idt */

/* external functions */
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();
extern void irqsyscall();

#endif /* _IDT_H */
