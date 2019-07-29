#include "pushad.h"
#include "console.h"
#include "sched.h"
#include "keybd.h"
#include "ide.h"

/* keep sync with usr/api.h */

void on_syscall(PUSHAD_ARGS)
{
	switch (eax) {
	case 1:
		setcolor(ecx);
		puts((const char *)edx);
		break;
	case 2:
		task_yield();
		break;
	case 3:
		asm volatile ("sti\nhlt\ncli");
		break;
	case 4:
		*&eax = getchar();
		break;
	case 5:
		ide_rdblk(ecx, edx, (void *)ebx);
		break;
	case 6:
		ide_wrblk(ecx, edx, (void *)ebx);
		break;
	}
}
