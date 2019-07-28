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

void __attribute__((noreturn)) __user_task_created(void *sp)
{
	move_to_user(sp, FL_1F | FL_IF);
}

struct task *create_user_task(void *proc, void *arg)
{
	char *user_stack = malloc(4096);
	setcolor(0xf);
	void **sp = (void *)user_stack + sizeof(user_stack);
	*--sp = arg;
	*--sp = 0;
	*--sp = proc;
	return create_task(__user_task_created, sp);
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
	extern void usr_echo_main(void); // in usr/echo.c
	task_join(create_user_task(usr_echo_main, "Hello, World!"));

	puts("\nEnabling Hardware Interrupt...\n");
	irq_setenable(0, 1);
	irq_setenable(1, 1);

	task_exit(0);
	/*asm volatile ("sti");
	for (;;) {
		asm volatile ("hlt");
	}*/
}
