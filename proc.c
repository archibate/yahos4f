#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/mman.h>
#include <linux/eflags.h>
#include <linux/vmm.h>
#include <stddef.h>

static int last_pid = 0;

int sys_getpid(void)
{
	return current->pid;
}

int sys_getppid(void)
{
	return current->ppid;
}

static int find_empty_task(void)
{
repeat:
	last_pid++;
	for (int i = 0; i < NR_TASKS; i++) {
		if (task[i] && task[i]->pid == last_pid)
			goto repeat;
	}
	for (int i = 1; i < NR_TASKS; i++) {
		if (!task[i])
			return i;
	}
	panic("failed to find_empty_task");
}

struct task *new_task(struct task *parent)
{
	int i = find_empty_task();
	struct task *p = calloc(1, sizeof(struct task));
	task[i] = p;
	p->stack = malloc(STACK_SIZE);
	p->mm = calloc(1, sizeof(struct mm));
	asm volatile ("mov %%cr3, %0" : "=r" (p->mm->pgd)); // todo: new_mm(p->mm)
	p->ppid = parent->pid;
	p->pid = last_pid;
	return p;
}

void __attribute__((noreturn)) sys_exit(int status)
{
	int i = get_pid_index(current->pid);
	if (i == -1)
		panic("cannot get current pid index");
	free(current);
	task[i] = current = NULL;
	schedule();
	panic("sys_exit() schedule returned");
}

static void __attribute__((noreturn)) __sys_exit(void)
{
	register int ret asm ("eax");
	sys_exit(ret);
}

struct task *setup_task(struct task *p, void *start, void *arg)
{
	void **sp = p->stack + STACK_SIZE;
	*--sp = arg;
	*--sp = __sys_exit;
	*--sp = start;
	p->ctx.sp = (long)sp;
	p->ctx.eflags = FL_1F;
	return p;
}
