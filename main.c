#include "console.h"
#include "gdt.h"
#include "idt.h"
#include "tss.h"
#include "pic.h"
#include "pit.h"
#include "mmu.h"
#include "mman.h"
#include "sched.h"
#include "user.h"
#include "eflags.h"

int test_proc(int arg)
{
	static char user_stack[8192];
	setcolor(0xf);
	puts("\nTest Thread Started!\nNow moving to user...\n");
	extern void usr_test_start(void); // in usr/test.asm
	move_to_user(usr_test_start, user_stack + sizeof(user_stack), FL_1F | FL_IF | (arg?FL_CF:0));
}

void main(void)
{
	clear();
	puts("Kernel Started...\n");
	//init_mmu();
	init_gdt();
	init_idt();
	init_pic();
	init_tss();
	set_timer_freq(10);

	init_mman();
	init_sched();
	task_join(create_task(test_proc, (void *)1));
	task_join(create_task(test_proc, (void *)0));

	puts("\nEnabling Hardware Interrupt...\n");
	irq_setenable(0, 1);
	irq_setenable(1, 1);

	asm volatile ("sti");
	for (;;) {
		setcolor(0x0f);
		puts(";");
		asm volatile ("hlt");
	}
}
