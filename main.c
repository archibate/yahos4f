#include "console.h"
#include "gdt.h"
#include "idt.h"
#include "tss.h"
#include "pic.h"
#include "pit.h"
#include "mmu.h"
#include "mman.h"
#include "sched.h"

int test_proc(void *arg)
{
	puts("\nTest Thread Started!\n");
	return 0;
}

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
	init_sched();
	struct task *test_task = create_task(test_proc, 0);
	task_run(test_task);

	puts("\nEnabling Hardware Interrupt...\n");
	irq_setenable(0, 1);
	irq_setenable(1, 1);
	asm volatile ("sti");
	for (;;) {
		asm volatile ("hlt");
	}
}
