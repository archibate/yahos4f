#include <linux/syscall.h>
#include <stddef.h>

int main(int argc, char **argv, char **envp);

void __attribute__((noreturn)) _start(int argc, char **argv, char **envp)
{
	_exit(main(argc, argv, envp));
}
