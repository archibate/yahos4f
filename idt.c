#include <linux/kernel.h>
#include <linux/idt.h>
#include <linux/keybd.h>
#include <linux/sched.h>
#include <linux/pic.h>

struct gatedesc idt[IDT_NR];

void on_divide_by_zero_error(void)
{
	panic("Int#0x00: Divide-By-Zero Error");
}

void asm_on_keyboard(void); /* In ientry.asm */
void on_keyboard(void)
{
	kb_handler();
	irq_done(1);
}

void asm_on_timer(void); /* In ientry.asm */
void on_timer(void)
{
	current->counter--;
	schedule();
	irq_done(0);
}

void asm_on_syscall(void); /* In ientry.asm */

void init_idt(void)
{
	SETGATE(idt[0x00], 0, 0x08, on_divide_by_zero_error, 0);
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
