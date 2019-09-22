#include <unistd.h>
#include <stdlib.h>
#include <linux/syscall.h>
#include <string.h>
#include <alloca.h>
#include <stdarg.h>

extern char **__envp;

int execve(const char *path, char *const *argv, char *const *envp)
{
	return _execve(path, argv, envp);
}

int execv(const char *path, char *const *argv)
{
	return execve(path, argv, __envp);
}

int execvpe(const char *file, char *const *argv, char *const *envp)
{
	int i;
	for (i = 0; file[i]; i++)
		if (file[i] == '/')
			return execv(file, argv);
	const char *prefix = getenv("PATH");
	if (!*prefix) return -1;
	do {
		for (i = 0; prefix[i] && prefix[i] != ':'; i++);
		char *path = alloca(i + 1 + strlen(file));
		memcpy(path, prefix, i);
		path[i] = '/';
		strcpy(path + i + 1, file);
		execve(path, argv, envp);
		prefix += i + 1;
	} while (prefix[-1]);
	return -1;
}

int execvp(const char *path, char *const *argv)
{
	return execvpe(path, argv, __envp);
}

#define EXECL_GETER(argv0) \
	va_list ap, pa; \
	va_start(ap, arg); \
	va_start(pa, arg); \
	int argv_size = 2, i = 1; \
	const char *gra = arg; \
	while (gra) { \
		argv_size++; \
		gra = va_arg(pa, const char *); \
	} \
	va_end(pa); \
	char **argv = alloca(argv_size * sizeof(char *)); \
	argv[0] = (char *)(argv0); \
	while (arg) { \
		argv[i++] = (char *)arg; \
		arg = va_arg(ap, const char *); \
	} \
	argv[i] = NULL;

int execle(const char *path, const char *arg, ...)
{
	EXECL_GETER(path);
	char *const *envp = va_arg(ap, char *const *);
	int ret = execve(path, argv, envp);
	va_end(ap);
	return ret;
}

int execl(const char *path, const char *arg, ...)
{
	EXECL_GETER(path);
	int ret = execv(path, argv);
	va_end(ap);
	return ret;
}

int execlp(const char *file, const char *arg, ...)
{
	EXECL_GETER(file);
	int ret = execvp(file, argv);
	va_end(ap);
	return ret;
}
