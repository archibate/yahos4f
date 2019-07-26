#include "sched.h"
#include "mman.h"
#include "eflags.h"
#include "console.h"

#define STACK_SIZE	8192

struct task *current;

void init_sched(void)
{
	current = malloc(sizeof(struct task));
	current->stack = 0;
}

void __attribute__((noreturn)) task_exit(int status)
{
	puts("task exited\n");
	for (;;)
		asm volatile ("cli\nhlt");
}

static void __task_return_proc(void)
{
	register int status asm ("eax");
	task_exit(status);
}

struct task *create_task(int (*proc)(void *), void *arg)
{
	void *stack = malloc(STACK_SIZE);
	void **sp = stack + STACK_SIZE;
	*--sp = arg;
	*--sp = __task_return_proc;
	*--sp = proc;

	struct task *task = malloc(sizeof(struct task));
	task->stack = stack;
	task->ctx.sp = (unsigned long)sp;
	task->ctx.eflags = FL_1F;
	return task;
}

void task_run(struct task *next)
{
	struct task *prev = current;
	current = next;
	switch_context(&prev->ctx, &current->ctx);
}
