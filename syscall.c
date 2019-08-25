#include <linux/kernel.h>
#include <linux/segment.h>
#include <linux/pushad.h>
#include <linux/sched.h>
#include <linux/fs.h>

static int sys_debug(const char __user *msg)
{
	debug(msg);
	return 0;
}

static int sys_mkdir(const char __user *path, unsigned int mode)
{
	return fs_mkdir(path, mode);
}

static int sys_mknod(const char __user *path, unsigned int mode, unsigned int nod)
{
	return fs_mknod(path, mode, nod);
}

static int sys_rmdir(const char __user *path)
{
	return fs_rmdir(path);
}

static int sys_link(const char __user *oldpath, const char __user *newpath)
{
	return fs_link(oldpath, newpath);
}

static int sys_unlink(const char __user *path)
{
	return fs_unlink(path);
}

void on_syscall(PUSHAD_ARGS)
{
	switch (eax) {
#define _SYSCALL_KERNEL_DEFINATION
#include <linux/syscall.h>
	default:
		warning("undefined syscall %d (%#x)", eax); /* ENOSYS */
	}
}
