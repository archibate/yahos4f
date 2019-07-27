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

int echo_proc(const char *msg)
{
	static char user_stack[8192];
	setcolor(0xf);
	puts("\nEcho Thread Started!\nNow moving to user...\n");
	void **sp = (void *)user_stack + sizeof(user_stack);
	puts("The user message is: ");
	setcolor(0xe);
	puts(msg);
	setcolor(0xf);
	puts("\n");
	*--sp = msg;
	*--sp = 0;
	extern void usr_echo_main(void); // in usr/echo.c
	move_to_user(usr_echo_main, sp, FL_1F | FL_IF);
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
	task_join(create_task(echo_proc, "Hello, World!"));

	puts("\nEnabling Hardware Interrupt...\n");
	irq_setenable(0, 1);
	irq_setenable(1, 1);

	task_exit(0);
	/*asm volatile ("sti");
	for (;;) {
		asm volatile ("hlt");
	}*/
}
