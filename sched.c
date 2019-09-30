#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/vmm.h>
#include <stddef.h>

struct task *task[NR_TASKS];
struct task *current;

void switch_to(int i)
{
	struct mm *previous_mm;
	struct task *previous;
	if (current != task[i]) {
		previous = current;
		current = task[i];
		printk("switch_diff %p->%p", previous, current);
		if (current->mm) {
			printk("switch_diff_mm...");
			use_mm(current->mm);
		}
		switch_context(&previous->ctx, &current->ctx);
	}
}

void schedule(void)
{
	int next;
	while (1) {
		next = 0;
		int c = -1;
		for (int i = NR_TASKS - 1; i >= 0; i--) {
			struct task *p = task[i];
			if (p->state == TASK_RUNNING && p->counter > c)
				c = p->counter, next = i;
		}
		if (c) break;
		for (int i = NR_TASKS - 1; i >= 0; i--) {
			struct task *p = task[i];
			if (p) p->counter = (p->counter >> 1) + p->priority;
		}
	}
	switch_to(next);
}

int sys_pause(void)
{
	current->state = TASK_INTRIB;
	schedule();
	return 0;
}

void sleep_on(struct task **p)
{
	struct task *old = *p;
	*p = current;
	current->state = TASK_UNINTRIB;
	schedule();
	if (old) old->state = 0;
}

void intrib_sleep_on(struct task **p)
{
	struct task *old = *p;
	*p = current;
repeat:	current->state = TASK_INTRIB;
	schedule();
	if (*p && *p != current) {
		(*p)->state = 0;
		goto repeat;
	}
	*p = NULL;
	if (old) old->state = 0;
}

void wake_up(struct task **p)
{
	if (!*p) return;
	(*p)->state = 0;
	*p = NULL;
}

int get_pid_index(int pid)
{
	for (int i = 0; i < NR_TASKS; i++)
		if (task[i]->pid == pid)
			return i;
	return -1;
}

void init_sched(void)
{
	static struct task initial_task = INITIAL_TASK;
	current = task[0] = &initial_task;
}
