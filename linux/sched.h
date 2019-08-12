#pragma once

#include "context.h"

#define TASK_RUNNING	0
#define TASK_INTRIB	1
#define TASK_UNINTRIB	2
#define TASK_ZOMBIE	3
#define TASK_STOPPED	4

struct task {
	int state;
	int counter;
	int priority;
	int pid, ppid;
	struct cont ctx;
#define STACK_SIZE	8192
	void *stack;
// user attributes:
	struct inode *cwd;
};

#define INITIAL_TASK	{ \
		.priority = 1, \
		.counter = 1, \
		.pid = 1, \
	}

#define NR_TASKS	64
extern struct task *task[NR_TASKS];
extern struct task *current;

void init_sched(void);
void schedule(void);
int sys_pause(void);
void sleep_on(struct task **p);
void intrib_sleep_on(struct task **p);
void wake_up(struct task **p);
int sys_getpid(void);
int sys_getppid(void);
struct task *new_task(struct task *parent);
struct task *setup_task(struct task *p, void *start, void *arg);
void __attribute__((noreturn)) sys_exit(int status);
