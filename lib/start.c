#include <linux/syscall.h>
#include <stddef.h>

int main(int argc, char **argv);

void __attribute__((noreturn)) _start(void)
{
	int argc = 0;
	char **argv = NULL;
	_exit(main(argc, argv));
}
