#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <alloca.h>

extern int debug(const char *msg);

#define MAX_ARGV 233

static int isblank(int c)
{
	return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}

int exit_stat;

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
			write(2, "arguments too much\n", 19);
			goto error;
		}
		argv[argc++] = s;
		if (is_end)
			break;
		s = t + 1;
	}

	int pid = fork();
	if (!pid) {
		exit_stat = execvp(argv[0], argv);
	} else if (pid > 0) {
		int i = 0;
		while (wait(&exit_stat) != pid);
		/*{
			char c[2] = {'\r', "\\|/-"[(i++ >> 14) & 3]};
			write(2, c, 2);
		}*/
	} else {
		exit_stat = pid;
	}

error:
	write(2, argv[0], strlen(argv[0]));
	write(2, ": exec error\n", 13);
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
			if (exit_stat) {
				char exit_char = exit_stat + '0';
				write(2, &exit_char, 1);
				exit_stat = 0;
			}
			write(2, "# ", 2);
		}

		char buf[256], *lp, *p = buf;
		if (read(fd, buf, sizeof(buf)) <= 0)
			break;

		while (*(lp = p)) {
			while (*p && *p != '\n') p++;
			*p = 0;
			command(lp);
			do p++; while (*p && *p == '\n');
		}
	}

	return 0;
}
