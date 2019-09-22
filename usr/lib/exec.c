#include <unistd.h>
#include <linux/syscall.h>

extern char **__envp;

int execve(const char *path, char *const *argv, char *const *envp)
{
	return _execve(path, argv, envp);
}

int execv(const char *path, char *const *argv)
{
	return _execve(path, argv, __envp);
}
