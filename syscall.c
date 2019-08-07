#include "pushad.h"
#include "sched.h"

/* keep sync with usr/api.h */

void on_syscall(PUSHAD_ARGS)
{
	switch (eax) {
	case 1:
		*&eax = sys_pause();
		break;
	case 2:
		*&eax = sys_getpid();
		break;
	case 3:
		*&eax = sys_getppid();
		break;
	}
}
