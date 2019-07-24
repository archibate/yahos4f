#include "console.h"
#include "gdt.h"

void main(void)
{
	puts("Kernel Started...\n");
	init_gdt();
	puts("\nGDT Initialized!\n");
}
