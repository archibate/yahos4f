#include "tss.h"
#include "gdt.h"
#include "idt.h"

static char ____temp_stack[2048];

struct tss tss0;

void init_tss(void)
{
	tss0.ts_ss0 = 0x10;
	tss0.ts_esp0 = (unsigned long)(____temp_stack + sizeof(____temp_stack));
	tss0.ts_iomb = sizeof(struct tss) - 1;
	tss0.ts_iomap[0] = 0xff;

	gdt[5] = SEGD16(STS_T32A, (unsigned long) &tss0, sizeof(struct tss), 0);
	gdt[5].sd_s = 0;

	asm volatile ("ltr %%ax" :: "a" (0x28));
}
