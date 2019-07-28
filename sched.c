#include "sched.h"
#include "mman.h"
#include "eflags.h"
#include "console.h"

struct task *current;

void init_sched(void)
{
	current = calloc(1, sizeof(struct task));
	current->next = current->prev = current;
}

void __attribute__((noreturn)) task_exit(int status)
{
	puts("task exited\n");
	for (;;)
		task_yield();
}

static void __task_return_proc(void)
{
	register int status asm ("eax");
	task_exit(status);
}

struct task *create_task(void *proc, void *arg1, void *arg2)
{
	void *stack = malloc(STACK_SIZE);
	struct task *task = calloc(1, sizeof(struct task));
	task->stack = stack;

	void **sp = stack + STACK_SIZE;
	*--sp = arg2;
	*--sp = arg1;
	*--sp = __task_return_proc;
	*--sp = proc;

	task->ctx.sp = (unsigned long)sp;
	task->ctx.eflags = FL_1F;
	task->prev = task->next = task;
	return task;
}

void task_join(struct task *task)
{
	task->next = current->next;
	current->next->prev = task;
	task->prev = current;
	current->next = task;
}

void task_run(struct task *next)
{
	struct task *prev = current;
	current = next;
	switch_context(&prev->ctx, &current->ctx);
}

void task_yield(void)
{
	task_run(current->next);
}
