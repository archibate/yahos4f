#include "console.h"
#include "gdt.h"
#include "idt.h"

void main(void)
{
	puts("Kernel Started...\n");
	init_gdt();
	puts("\nGDT Initialized!\n");
	init_idt();
	puts("\nIDT Initialized!\n");
	puts("\nDoing Soft Interrupt...");
	asm volatile ("int $0x80");
	puts("\nSoft Interrupt Returned!\n");
}
