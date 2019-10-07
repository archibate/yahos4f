#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
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
			perror("cd");
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

	} else if (!strcmp(argv[0], "exec")) {
		return 1;

	} else if (!strcmp(argv[0], "echo")) {
		for (int i = 1; argv[i]; i++) {
			if (i != 1) fputc(' ', stdout);
			fputs(argv[i], stdout);
		}
		fputc('\n', stdout);
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
	fprintf(stderr, "%s: bad arguments\n", argv[0]);
	exit_stat = -1;
	return 0;
}

int execute_command(char **argv)
{
	int i = try_internal_command(argv);
	if (i == 1) {
		argv[0] = argv[1];
		argv++;
		goto do_exec;
	} else if (i != -1)
		return i;

	int pid = fork();
	if (!pid) {
do_exec:
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
			fprintf(stderr, "%s: too much arguments\n", argv[0]);
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

	if (i == 1)
		fprintf(stderr, "%s: bad command\n", argv[0]);
	else
		fprintf(stderr, "%s: bad fork\n", argv[0]);
}

int main(int argc, char **argv)
{
	FILE *fp = argv[1] ? fopen(argv[1], "r") : stdin;
	if (!fp) {
		perror(argv[1] ? argv[1] : "<stdin>");
		return EXIT_FAILURE;
	}

	while (1) { // TODO: too bad, use fgets instead
		if (fp == stdin) {
			fprintf(stderr, "# ");
			fflush(stderr);
		}

		char buf[256], *lp, *p = buf;
		if (!fgets(buf, sizeof(buf), fp))
			break;

		while (*(lp = p)) {
			while (*p && *p != '\n') p++;
			*p = 0;
			command(lp);
			do p++; while (*p && *p == '\n');
		}
	}

	return EXIT_SUCCESS;
}
