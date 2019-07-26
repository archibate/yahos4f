#include "console.h"
#include "gdt.h"
#include "idt.h"
#include "tss.h"
#include "pic.h"
#include "pit.h"
#include "mmu.h"
#include "mman.h"

void main(void)
{
	clear();
	puts("Kernel Started...\n");
	init_mmu();
	init_gdt();
	init_idt();
	init_pic();
	init_tss();
	set_timer_freq(10);

	init_mman();
	void *p = malloc(10);
	void *q = malloc(20);
	free(p);
	free(q);

	puts("\nEnabling Hardware Interrupt...\n");
	irq_setenable(0, 1);
	irq_setenable(1, 1);
	asm volatile ("sti");
	for (;;) {
		asm volatile ("hlt");
	}
}
