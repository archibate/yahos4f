#include <linux/syscall.h>
#include <fcntl.h>
#include <string.h>

void cat(const char *path)
{
	int fd = path ? _open(path, O_RDONLY) : 0;

	while (1) {
		char buf[7];
		memset(buf, 0, sizeof(buf));
		if (_read(0, buf, sizeof(buf) - 1) <= 0)
			break;
		_write(fd, buf, strlen(buf));
	}

	if (fd) _close(fd);
}

int main(int argc, char **argv)
{
	if (argc == 1) cat(NULL);
	else for (int i = 1; i < argc; i++)
		cat(argv[i]);
	return 0;
}
