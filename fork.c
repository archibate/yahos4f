#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/vmm.h>
#include <malloc.h>
#include <string.h>

static struct file *fork_file_table(struct file *old_ft)
{
	struct file *new_ft = calloc(NR_OPEN, sizeof(struct file));
	memcpy(new_ft, old_ft, NR_OPEN * sizeof(struct file));
	for (int i = 0; i < NR_OPEN; i++) {
		new_ft[i].f_inode = idup(old_ft[i].f_inode);
	}
	return new_ft;
}

static int do_fork_from_to(struct task *old, struct task *new)
{
	new->mm = fork_mm(old->mm);
	if (old->executable)
		new->executable = idup(old->executable);
	if (old->cwd)
		new->cwd = idup(old->cwd);
	new->euid = old->euid;
	new->egid = old->egid;
	new->ebss = old->ebss;
	new->brk = old->brk;
	new->stop = old->stop;
	new->file = fork_file_table(old->file);
	new->priority = old->priority;
	memcpy(new->stack + STACK_SIZE - 4*13,
	       old->stack + STACK_SIZE - 4*13, 4*13);
}

static int run_fork(void)
{
	extern void __attribute__((noreturn)) fork_child_return(void *uctx);
	fork_child_return(current->stack + STACK_SIZE - 4*13); // user context
}

int sys_fork(void)
{
	struct task *p = new_task(current);
	setup_kernel_task(p, run_fork, NULL);
	do_fork_from_to(current, p);
	return p->pid;
}
