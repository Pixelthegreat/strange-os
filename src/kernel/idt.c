#include "idt.h" /* header file */
#include "util.h" /* utilities */
#include "../types.h" /* custom types */
#include "../include/unistd.h" /* unistd io */
#include "../include/string.h" /* string */
#include "kprint.h" /* terminal */
#include "syscall.h" /* system calls */
#include "disk/ata.h" /* ata stuff */

/* values */
idt_reg_t idt_reg;
idt_gate idt[256];
isr int_handlers[256];

/* forward decls */
static void no_irq(registers *);

/* set a gate */
extern void set_idt_gate(int n, u32 handler) {

	idt[n].low_offset = low16(handler);
	idt[n].selector = 0x08;
	idt[n].always0 = 0;
	idt[n].flags = 0x8E;
	idt[n].high_offset = high16(handler);
}

/* handle an isr */
extern void isr_handler(registers *r) {

	/* print an error message */
	kprint("error: cpu error ");
	kprinthex(r->int_no);
	kprintnl();
}

/* handle an irq */
extern void irq_handler(registers *r) {

	/* get the handler */
	if (int_handlers[r->int_no] != NULL) {

		isr handler = int_handlers[r->int_no];
		handler(r);
	}

	/* eoi (for PIC irqs only) */
	if (r->int_no < 48) {
	
		portbout(0x20, 0x20);
		if (r->int_no >= 40)
			portbout(0xA0, 0x20);
	}
}

/* handle any regular interrupt */
extern void int_handler(registers *r) {

	/* get the handler */
	if (int_handlers[r->int_no] != NULL) {
	
		isr handler = int_handlers[r->int_no];
		handler(r);
	}
	
	/* done */
}

/* register a handler */
extern void idt_set_handler(u32 n, isr handler) {

	/* set handler */
	int_handlers[n] = handler;
}

/* load the idt */
extern void idt_load(void) {

	idt_reg.base = (u32)&idt;
	idt_reg.limit = sizeof(idt) - 1;
	asm volatile("lidt (%0)" : : "r" (&idt_reg));
	
	/* load the keyboard interrupt */
	idt_set_handler(33, kbint);

	/* system calls */
	idt_set_handler(0x40, ksysint);
	
	/* load ata interrupt (both 46 and 47 are used for this) */
	idt_set_handler(46, ata_irq);
	idt_set_handler(47, no_irq);
}

/* no irq */
static void no_irq(registers *r) {
}

/* setup isrs */
extern void isr_install(void) {

	set_idt_gate(0, (u32)isr0);
	set_idt_gate(1, (u32)isr1);
	set_idt_gate(2, (u32)isr2);
	set_idt_gate(3, (u32)isr3);
	set_idt_gate(4, (u32)isr4);
	set_idt_gate(5, (u32)isr5);
	set_idt_gate(6, (u32)isr6);
	set_idt_gate(7, (u32)isr7);
	set_idt_gate(8, (u32)isr8);
	set_idt_gate(9, (u32)isr9);
	set_idt_gate(10, (u32)isr10);
	set_idt_gate(11, (u32)isr11);
	set_idt_gate(12, (u32)isr12);
	set_idt_gate(13, (u32)isr13);
	set_idt_gate(14, (u32)isr14);
	set_idt_gate(15, (u32)isr15);
	set_idt_gate(16, (u32)isr16);
	set_idt_gate(17, (u32)isr17);
	set_idt_gate(18, (u32)isr18);
	set_idt_gate(19, (u32)isr19);
	set_idt_gate(20, (u32)isr20);
	set_idt_gate(21, (u32)isr21);
	set_idt_gate(22, (u32)isr22);
	set_idt_gate(23, (u32)isr23);
	set_idt_gate(24, (u32)isr24);
	set_idt_gate(25, (u32)isr25);
	set_idt_gate(26, (u32)isr26);
	set_idt_gate(27, (u32)isr27);
	set_idt_gate(28, (u32)isr28);
	set_idt_gate(29, (u32)isr29);
	set_idt_gate(30, (u32)isr30);
	set_idt_gate(31, (u32)isr31);
	
	/* reprogram the pic (if you want to learn more, you should go check out the osdev wiki page "8259 PIC") */
	portbout(0x20, 0x11);
	io_wait();
	portbout(0xA0, 0x11);
	io_wait();
	
	// ICW2
	portbout(0x21, 0x20);
	io_wait();
	portbout(0xA1, 0x28);
	io_wait();
	
	// ICW3
	portbout(0x21, 0x04);
	io_wait();
	portbout(0xA1, 0x02);
	io_wait();
	
	// ICW4
	portbout(0x21, 0x01);
	io_wait();
	portbout(0xA1, 0x01);
	io_wait();
	
	// OCW1
	portbout(0x21, 0x0);
	io_wait();
	portbout(0xA1, 0x0);
	io_wait();

	/* set irqs */
	set_idt_gate(32, (u32)irq0);
	set_idt_gate(33, (u32)irq1);
	set_idt_gate(34, (u32)irq2);
	set_idt_gate(35, (u32)irq3);
	set_idt_gate(36, (u32)irq4);
	set_idt_gate(37, (u32)irq5);
	set_idt_gate(38, (u32)irq6);
	set_idt_gate(39, (u32)irq7);
	set_idt_gate(40, (u32)irq8);
	set_idt_gate(41, (u32)irq9);
	set_idt_gate(42, (u32)irq10);
	set_idt_gate(43, (u32)irq11);
	set_idt_gate(44, (u32)irq12);
	set_idt_gate(45, (u32)irq13);
	set_idt_gate(46, (u32)irq14);
	set_idt_gate(47, (u32)irq15);
	
	/* set syscall gate */
	set_idt_gate(0x40, (u32)irqsyscall);

	/* load the idt */
	idt_load();

	/* enable external interrupts */
	isr_enable();
}

/* enable isrs */
extern void isr_enable(void) {

	/* enable interrupts */
	asm volatile("sti");
}
