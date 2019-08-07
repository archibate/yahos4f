#pragma once

#include "context.h"

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
struct task *create_task(void *proc, void *arg1, void *arg2);
