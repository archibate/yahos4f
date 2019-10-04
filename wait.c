#include <linux/kernel.h>
#include <linux/sched.h>
#include <stddef.h>

static struct task *exit_notify, *exiter;
static int exit_stat;

void __attribute__((noreturn)) sys_exit(int status)
{
	current->exit_stat = status;
	current->state = TASK_ZOMBIE;
	exiter = current;
	current = NULL;
	exit_stat = status;
	wake_up(&exit_notify);
	schedule();
	panic("sys_exit() schedule returned");
}

int sched_wait(int *stat_loc)
{
	while (exiter->ppid != current->pid)
		sleep_on(&exit_notify);
	*stat_loc = exit_stat;
	int pid = exiter->pid;
	destroy_task(exiter);
	exiter = NULL;
	return pid;
}
