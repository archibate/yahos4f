#include "console.h"
#include "gdt.h"
#include "idt.h"
#include "pic.h"
#include "pit.h"

void main(void)
{
	puts("Kernel Started...\n");
	init_gdt();
	puts("\nGDT Initialized\n");
	init_idt();
	puts("\nIDT Initialized\n");
	init_pic();
	puts("\nPIC Initialized\n");
	set_timer_freq(10);
	puts("\nInitialized Timer Frequency to 10Hz\n");

	puts("\nEnabling Hardware Interrupt...\n");
	irq_setenable(0, 1);
	irq_setenable(1, 1);
	asm volatile ("sti");
	for (;;) {
		asm volatile ("hlt");
	}
}
