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

void exec_user(void *proc, void *arg)
{
#define USER_STACK_SIZE	4096
	char *user_stack = malloc(USER_STACK_SIZE);
	void **sp = (void *)user_stack + USER_STACK_SIZE;
	*--sp = arg;
	*--sp = 0;
	*--sp = proc;
	move_to_user(sp, FL_1F | FL_IF);
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
	extern void usr_echo_main(); // in usr/echo.c
	extern void usr_chkdsk_main(); // in usr/chkdsk.c
	task_join(create_task(exec_user, usr_chkdsk_main, 0));
	task_join(create_task(exec_user, usr_echo_main, "\a00"));
	task_join(create_task(exec_user, usr_echo_main, "\a11"));
	task_join(create_task(exec_user, usr_echo_main, "\a22"));
	task_join(create_task(exec_user, usr_echo_main, "\a33"));
	task_join(create_task(exec_user, usr_echo_main, "\a44"));
	task_join(create_task(exec_user, usr_echo_main, "\a55"));
	task_join(create_task(exec_user, usr_echo_main, "\a66"));
	task_join(create_task(exec_user, usr_echo_main, "\a77"));

	puts("\nEnabling Hardware Interrupt...\n");
	irq_setenable(0, 1);
	irq_setenable(1, 1);

	task_exit(0);
	/*asm volatile ("sti");
	for (;;) {
		asm volatile ("hlt");
	}*/
}
