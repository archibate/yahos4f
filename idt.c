#include "idt.h"
#include "console.h"
#include "sched.h"
#include "keybd.h"
#include "pic.h"

struct gatedesc idt[IDT_NR];

void on_divide_by_zero_error(void)
{
	puts("Int#0x00: Divide-By-Zero Error Occurred!\n");
	asm volatile ("cli\nhlt");
}

void asm_on_soft_interrupt(void); /* In ientry.asm */
void on_soft_interrupt(void)
{
	puts("Int#0x80: Soft Interrupt Called!\n");
}

void asm_on_keyboard(void); /* In ientry.asm */
void on_keyboard(void)
{
	//puts("Int#0x21: Keyboard Interrupt!\n");
	kb_handler();
	irq_done(1);
}

void asm_on_timer(void); /* In ientry.asm */
void on_timer(void)
{
	task_yield();
	irq_done(0);
	//puts("Int#0x20: Timer Interrupt!\n");
}

void asm_on_syscall(void); /* In ientry.asm */
void on_syscall(void)
{
	//task_yield();
	//irq_done(0);
	//puts("Int#0x80: System Call\n");
	setcolor(0xc);
	puts("O");
	asm volatile ("sti\nhlt\ncli");
}

void init_idt(void)
{
	SETGATE(idt[0x00], 0, 0x08, on_divide_by_zero_error, 0);
	SETGATE(idt[0x80], 0, 0x08, asm_on_soft_interrupt, 0);
	SETGATE(idt[0x21], 0, 0x08, asm_on_keyboard, 0);
	SETGATE(idt[0x20], 0, 0x08, asm_on_timer, 0);
	SETGATE(idt[0x80], 0, 0x08, asm_on_syscall, 3);
	static struct idtr {
		unsigned short limit;
		unsigned long base;
	} __attribute__((packed)) idtr = {
		.limit = sizeof(idt) - 1,
		.base = (unsigned long) idt,
	};
	asm volatile ("lidt (%0)" :: "r" (&idtr));
}
