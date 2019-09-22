#include <linux/syscall.h>
#include <stddef.h>

char **__envp;

int main(int argc, char **argv, char **envp);

void __attribute__((noreturn)) _start(int argc, char **argv, char **envp)
{
	__envp = envp;
	_exit(main(argc, argv, envp));
}
