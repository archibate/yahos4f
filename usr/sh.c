#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <alloca.h>
#include <errno.h>

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

	} else {
		return -1;
	}
error:
	fprintf(stderr, "%s: bad arguments\n", argv[0]);
	exit_stat = -1;
	return 0;
}

int try_builtin_command(char *const *argv)
{
	exit_stat = 0;
	if (0) {
	} else if (!strcmp(argv[0], "set")) {
		if (!argv[1] || argv[2])
			goto error;
		putenv(argv[1]);
		return 0;

	} else if (!strcmp(argv[0], "unset")) {
		if (!argv[1] || argv[2])
			goto error;
		unsetenv(argv[1]);
		return 0;

	} else if (!strcmp(argv[0], "echo")) {
		for (int i = 1; argv[i]; i++) {
			if (i != 1) fputc(' ', stdout);
			fputs(argv[i], stdout);
		}
		fputc('\n', stdout);
		return 0;

	} else if (!strcmp(argv[0], "get")) {
		if (!argv[1] || argv[2])
			goto error;
		char *value = getenv(argv[1]);
		if (value) {
			puts(value);
		}
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
	if (argv[1] && !strcmp(argv[1], "exec")) {
		argv[0] = argv[1];
		argv++;
		goto do_exec;
	}
	if (-1 != try_internal_command(argv))
		return 0;

	int pid = fork();
	if (!pid) {
		if (-1 != try_builtin_command(argv))
			exit(exit_stat);
do_exec:
		exit_stat = execvp(argv[0], argv);
		if (errno == ENOENT)
			fprintf(stderr, "%s: command not found\n", argv[0]);
		else
			perror(argv[0]);
		return 1;
	} else if (pid > 0) {
		int i = 0;
		while (wait(&exit_stat) != pid);
		return 0;
	} else {
		exit_stat = pid;
		perror("fork");
		return 2;
	}
}

char *do_token(char **s)
{
	while (**s && isblank(**s)) ++*s;
	if (!**s)
		return NULL;
	char *p = *s;
	while (**s && !isblank(**s)) ++*s;
	if (**s) *(*s)++ = 0;
	return p;
}

void command(char *s)
{
	int argc = 0;
	char *argv[MAX_ARGV], *arg;
	if ((arg = strchr(s, '#')))
		*arg = 0;
	while ((arg = do_token(&s))) {
		if (argc == MAX_ARGV + 1) {
			fprintf(stderr, "sh: too much arguments\n");
			return;
		}
		argv[argc++] = arg;
	}
	if (!argc)
		return;
	argv[argc] = 0;
	execute_command(argv);
}

size_t rline(char *buf, size_t size)
{
	buf = fgets(buf, size, stdin);
	return buf ? strlen(buf) : 0;
#if 0
	char c, *p = buf;
	while (p + 1 < buf + size) {
		c = fgetc(stdin);
		*p++ = c;
		if (c == EOF)
			break;
		if (c == '\n')
			break;
	}
	*p = 0;
	return p - buf;
#endif
}

int main(int argc, char **argv)
{
	FILE *fp = argv[1] ? fopen(argv[1], "r") : stdin;
	if (!fp) {
		perror(argv[1] ? argv[1] : "<stdin>");
		return EXIT_FAILURE;
	}
	int is_tty = fp == stdin; // TODO: use termios.h function instead!

	while (1) {
		char buf[256];

		if (is_tty) {
			fprintf(stderr, "# ");
			fflush(stderr);
			if (!rline(buf, sizeof(buf)))
				break;
		} else if (!fgets(buf, sizeof(buf), fp))
			break;

		char *lp, *p = buf;
		while (*(lp = p)) {
			while (*p && *p != '\n') p++;
			*p = 0;
			command(lp);
			do p++; while (*p && *p == '\n');
		}
	}

	return EXIT_SUCCESS;
}
