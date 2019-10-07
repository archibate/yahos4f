#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>

char **environ;
int _environ_size = 0;

extern int main(int argc, char **argv, char **envp);
extern void __stdio_init_buffers(void);

void libc_init(int argc, char **argv, char **envp)
{
	environ = envp;
	while (*envp) {
		_environ_size++;
		envp++;
	}

	__stdio_init_buffers();
}

void libc_finish(void)
{
}

void __attribute__((noreturn)) _start(int argc, char **argv, char **envp)
{
	libc_init(argc, argv, envp);
	exit(main(argc, argv, envp));
}

void __attribute__((noreturn)) exit(int status)
{
	libc_finish();
	_exit(status);
}
