#include <linux/syscall.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char **argv, char **envp)
{
	_open("/dev/tty", O_RDONLY);
	_open("/dev/tty", O_WRONLY);
	_dup(1);

	_write(1, "init started\n", 13);

	char *argv_run[] = {"/bin/env", NULL};
	_execve(argv_run[0], argv_run, envp);

	return 0;
}
