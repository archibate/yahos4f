#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
	chdir("/root");

	open("/dev/tty", O_RDONLY);
	open("/dev/tty", O_WRONLY);
	dup(1);

	write(2, "init started\n", 13);
	int pid = fork();
	if (pid == 0) {
		execl("/bin/sh", "sh", "/etc/rcS", NULL);
		write(2, "exec error\n", 11);
		return -1;
	} else if (pid < 0) {
		write(2, "fork error\n", 11);
		return -1;
	}
	int stat;
	while (wait(&stat) != pid);

	write(2, "init exiting...\n", 16);
	return stat;
}
