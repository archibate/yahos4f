#include "idt.h"
#include "console.h"

struct gatedesc idt[IDT_NR];

void on_divide_by_zero_error(void)
{
	puts("Int#0x00: Divide-By-Zero Error Occurred!\n");
	asm volatile ("cli\nhlt");
}

void asm_on_soft_interrupt(void); /* In ientry.asm */
void on_soft_interrupt(void)
{
	puts("Int#0x00: Soft Interrupt Called!\n");
}

void init_idt(void)
{
	SETGATE(idt[0x00], 0, 0x08, on_divide_by_zero_error, 0);
	SETGATE(idt[0x80], 0, 0x08, asm_on_soft_interrupt, 0);
	static struct idtr {
		unsigned short limit;
		unsigned long base;
	} __attribute__((packed)) idtr = {
		.limit = sizeof(idt) - 1,
		.base = (unsigned long) idt,
	};
	asm volatile ("lidt (%0)" :: "r" (&idtr));
}
