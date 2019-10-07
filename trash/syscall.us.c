#include <linux/kernel.h>
#include <linux/segment.h>
#include <linux/pushad.h>
#include <linux/sched.h>
#include <linux/fs.h>

static int sys_debug(const char __user *msg)
{
	U_STRING_BEGIN(msg);
	debug(msg);
	U_STRING_END(msg);
	return 0;
}

static int sys_mkdir(const char __user *path, unsigned int mode)
{
	U_STRING_BEGIN(path);
	int ret = fs_mkdir(path, mode);
	U_STRING_END(path);
	return ret;
}

static int sys_rmdir(const char __user *path)
{
	U_STRING_BEGIN(path);
	int ret = fs_rmdir(path);
	U_STRING_END(path);
	return ret;
}

static int sys_link(const char __user *oldpath, const char __user *newpath)
{
	U_STRING_BEGIN(oldpath);
	U_STRING_BEGIN(newpath);
	int ret = fs_link(oldpath, newpath);
	U_STRING_END(newpath);
	U_STRING_END(oldpath);
	return ret;
}

static int sys_unlink(const char __user *path)
{
	U_STRING_BEGIN(path);
	int ret = fs_unlink(path);
	U_STRING_END(path);
	return ret;
}

void on_syscall(PUSHAD_ARGS)
{
	switch (eax) {
#define _SYSCALL_KERNEL_DEFINATION
#include <linux/syscall.h>
	}
}
