#include <linux/syscall.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv)
{
	_chdir("/root");

	_open("/dev/tty", O_RDONLY);
	_open("/dev/tty", O_WRONLY);
	_dup(1);

	_write(1, "init started\n", 13);

	char *argv_run[] = {"/bin/cat", "../etc/issue", NULL};
	execv(argv_run[0], argv_run);

	return 0;
}
