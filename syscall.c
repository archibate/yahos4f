#include <linux/kernel.h>
#include <linux/segment.h>
#include <linux/pushad.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/vmm.h>
#include <linux/io.h>
#include <string.h>
#include <alloca.h>
#include <errno.h>

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

static int sys_execve(const char __user *path,
		char __user *const __user *argv,
		char __user *const __user *envp)
{
	char *path_sys = alloca(strlen(path) + 1);
	strcpy(path_sys, path);
	int argc, envc;
	for (argc = 0; argv[argc]; argc++);
	for (envc = 0; envp[envc]; envc++);
	char *argv_sys[argc + 1], *envp_sys[envc + 1];
	argv_sys[argc] = envp_sys[envc] = NULL;
	for (int i = 0; i < argc; i++) {
		argv_sys[i] = alloca(strlen(argv[i]) + 1);
		strcpy(argv_sys[i], argv[i]);
	}
	for (int i = 0; i < envc; i++) {
		envp_sys[i] = alloca(strlen(envp[i]) + 1);
		strcpy(envp_sys[i], envp[i]);
	}
	return do_execve(path_sys, argv_sys, envp_sys);
}

static int sys_chdir(const char __user *path)
{
	struct inode *ip = namei(path);
	if (!ip) return -1;
	if (!S_ISDIR(ip->i_mode)) {
		errno = ENOTDIR;
		return -1;
	}
	if (current->cwd)
		iput(current->cwd);
	current->cwd = ip;
	return 0;
}

#if 0
static char __user *sys_getcwd(char __user *path, size_t size)
{
	if (!current->cwd_string)
		return NULL;
	size_t real_size = strlen(current->cwd_string) + 1;
	if (real_size > size)
		return NULL; /* ERANGE */
	memcpy(path, current->cwd_string, real_size);
}
#endif

static int sys_wait(int __user *stat_loc)
{
	return sched_wait(stat_loc);
}

static void __user *sys_sbrk(int incr)
{
	void __user *ret = (void *)current->brk;
	current->brk += incr;
	return ret;
}

static int sys_brk(void __user *addr)
{
	return -(sys_sbrk(addr - (void *)current->brk) == (void *)-1);
}

static int sys_reboot(int cmd)
{
	outb(0x64, 0xfe);
	return 0;
}

void on_syscall(PUSHAD_ARGS)
{
#define _SYSCALL_KERNEL_DEFINATION
#include <linux/syscall.h>
}
