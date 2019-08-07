#include "console.h"
#include "gdt.h"
#include "idt.h"
#include "tss.h"
#include "pic.h"
#include "pit.h"
#include "mmu.h"
#include "mman.h"
#include "sched.h"
#include "time.h"

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

	asm volatile ("sti");
	for (;;) {
		struct tm t;
		cmos_gettime(&t);
		printf("\r%02d/%02d/%02d %02d:%02d:%02d",
				t.tm_year, t.tm_mon, t.tm_mday,
				t.tm_hour, t.tm_min, t.tm_sec);

		asm volatile ("hlt");
	}
}
