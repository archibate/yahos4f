#include "console.h"
#include "gdt.h"
#include "idt.h"
#include "tss.h"
#include "pic.h"
#include "pit.h"
#include "mmu.h"
#include "mman.h"
#include "sched.h"
#include "keybd.h"

void func(int i)
{
	while (1) {
		setcolor(0x8 + i);
		char c[2];
		c[0] = '0' + i;
		c[1] = 0;
		puts(c);
		for (volatile int j = 0; j < 500000; j++);
	}
}
void main(void)
{
	clear();
	puts("Kernel Started...\n");
	init_gdt();
	init_idt();
	init_pic();
	init_tss();
	set_timer_freq(10);
	init_mman();
	init_sched();

	irq_setenable(0, 1);
	irq_setenable(1, 1);

	for (int i = 0; i < 8; i++) {
		setup_task(new_task(current), func, (void*)i)->priority = 1;
	}

	asm volatile ("sti");
	for (;;) {
		asm volatile ("hlt");
	}
}
