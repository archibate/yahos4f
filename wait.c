#include <linux/kernel.h>
#include <linux/sched.h>

int sys_wait(void)
{
	sys_pause();
	return 0;
}
