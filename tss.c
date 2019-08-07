#include <linux/tss.h>
#include <linux/gdt.h>
#include <linux/idt.h>
#include <linux/sched.h>

static char ____tmp_kstack[8192];

struct tss tss0;

void init_tss(void)
{
	tss0.ts_ss0 = 0x10;
	tss0.ts_esp0 = (unsigned long)(____tmp_kstack + sizeof(____tmp_kstack));
	tss0.ts_iomb = sizeof(struct tss) - 1;
	tss0.ts_iomap[0] = 0xff;

	gdt[5] = SEGD16(STS_T32A, (unsigned long)&tss0, sizeof(struct tss), 0);
	gdt[5].sd_s = 0;

	asm volatile ("ltr %%ax" :: "a" (0x28));
}

void __int_leave(void)
{
	tss0.ts_esp0 = (unsigned long)(current->stack + STACK_SIZE);
}
