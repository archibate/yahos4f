#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int system(const char *s)
{
	int pid = fork();
	if (pid == 0) {
		return execl("/bin/sh", "sh", "-c", s, NULL);
	} else if (pid > 0) {
		int stat;
		while (wait(&stat) != pid);
		return stat;
	} else {
		return pid;
	}
}
