#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <alloca.h>

extern int debug(const char *msg);

#define MAX_ARGV 233

static int isblank(int c)
{
	return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}

int exit_stat;

int try_internal_command(char *const *argv)
{
	exit_stat = 0;
	if (0) {
	} else if (!strcmp(argv[0], "cd")) {
		if (!argv[1] || argv[2])
			goto error;
		if (chdir(argv[1]) == -1) {
			write(2, "cd: chdir error\n", 16);
			exit_stat = -1;
		}
		return 0;

	} else if (!strcmp(argv[0], "exit")) {
		if (argv[1]) {
			if (argv[2])
				goto error;
			exit_stat = atoi(argv[1]);
		}
		exit(exit_stat);
		return 0;

	} else if (!strcmp(argv[0], "echo")) {
		for (int i = 1; argv[i]; i++) {
			if (i != 1) write(1, " ", 1);
			write(1, argv[i], strlen(argv[i]));
		}
		write(1, "\n", 1);
		return 0;

	} else if (!strcmp(argv[0], "true")) {
		exit_stat = 0;
		return 0;

	} else if (!strcmp(argv[0], "false")) {
		exit_stat = 1;
		return 0;

	} else {
		return -1;
	}
error:
	write(2, argv[0], strlen(argv[0]));
	write(2, ": bad arguments\n", 16);
	exit_stat = -1;
	return 0;
}

int execute_command(char *const *argv)
{
	int i = try_internal_command(argv);
	if (i != -1)
		return i;

	int pid = fork();
	if (!pid) {
		exit_stat = execvp(argv[0], argv);
		return 1;
	} else if (pid > 0) {
		int i = 0;
		while (wait(&exit_stat) != pid);
		return 0;
	} else {
		exit_stat = pid;
		return 2;
	}
}

void command(char *s)
{
	char *argv[MAX_ARGV];
	int argc = 0;
	exit_stat = 0;
	while (*s && isblank(*s))
		s++;
	while (1) {
		while (*s && isblank(*s))
			s++;
		if (!*s)
			break;
		char *t = s;
		while (*t && !isblank(*t))
			t++;
		int is_end = !*t;
		*t = 0;
		if (argc >= MAX_ARGV) {
			write(2, "too much arguments\n", 19);
			return;
		}
		argv[argc++] = s;
		if (is_end)
			break;
		s = t + 1;
	}
	argv[argc] = NULL;

	int i = execute_command(argv);
	if (!i) return;

	write(2, argv[0], strlen(argv[0]));
	if (i == 1)
		write(2, ": bad command\n", 14);
	else
		write(2, ": bad execve\n", 13);
}

int main(int argc, char **argv)
{
	int fd = argv[1] ? open(argv[1], O_RDONLY) : 0;
	if (fd == -1) {
		write(2, argv[1], strlen(argv[1]));
		write(2, ": open error\n", 13);
		return 1;
	}

	while (1) { // TODO: too bad, use fgets instead
		if (!fd) {
			write(2, "# ", 2);
		}

		char buf[256], *lp, *p = buf;
		int size_read = read(fd, buf, sizeof(buf) - 1);
		if (size_read <= 0)
			break;
		buf[size_read] = 0;

		while (*(lp = p)) {
			while (*p && *p != '\n') p++;
			*p = 0;
			command(lp);
			do p++; while (*p && *p == '\n');
		}
	}

	return 0;
}
