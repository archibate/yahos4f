#pragma once

struct cont
{
	unsigned long sp;
	unsigned long bx, si, di, bp;
	unsigned long eflags;
} __attribute__((packed));

void __attribute__((fastcall)) switch_context(struct cont *prev, struct cont *next);

struct task
{
	struct task *next, *prev;
	struct cont ctx;
#define STACK_SIZE	8192
	void *stack;
};

extern struct task *current;

void init_sched(void);
void task_yield(void);
void task_join(struct task *next);
void task_run(struct task *next);
void __attribute__((noreturn)) task_exit(int status);
struct task *create_task(void *proc, void *arg);
