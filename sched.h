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
	struct cont ctx;
	void *stack;
};

void init_sched(void);
void task_run(struct task *next);
void __attribute__((noreturn)) task_exit(int status);
struct task *create_task(int (*proc)(void *), void *arg);
