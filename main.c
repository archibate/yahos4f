#include "console.h"
#include "gdt.h"
#include "idt.h"
#include "pic.h"

void main(void)
{
	puts("Kernel Started...\n");
	init_gdt();
	puts("\nGDT Initialized!\n");
	init_idt();
	puts("\nIDT Initialized!\n");
	init_pic();
	puts("\nPIC Initialized!\n");
	puts("\nEnabling Hardware Interrupt...\n");
	irq_setenable(1, 1);
	asm volatile ("sti\nhlt");
	puts("Hardware Interrupt Received!\n");
}
