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
	puts("\nDoing 1/0 Test...");
	volatile int x = 0;
	volatile int y = 1 / x;
	puts("\n1/0 Test Done (should not be)\n");
}
