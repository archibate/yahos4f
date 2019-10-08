#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

extern int reboot(int cmd); // my system api da!

int main(int argc, char **argv)
{
	chdir("/root");

	open("/dev/tty", O_RDONLY);
	open("/dev/tty", O_WRONLY);
	dup(1);

	printf("init started\n");
	int pid = fork();
	if (pid == 0) {
		execl("/bin/sh", "sh", "/etc/rcS", NULL);
		perror("/bin/sh");
		return -1;
	} else if (pid < 0) {
		perror("fork");
		return -1;
	}
	int stat;
	while (wait(&stat) != pid);

	printf("init shell exited %d\n", stat);
	reboot(233);
	return stat;
}
