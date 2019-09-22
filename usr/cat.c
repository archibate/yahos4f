#include <linux/syscall.h>
#include <fcntl.h>
#include <string.h>

void cat(const char *path)
{
	int fd = path ? _open(path, O_RDONLY) : 0;
	if (fd == -1) {
		_write(2, path, strlen(path));
		_write(2, ": open error\n", 13);
		return;
	}

	while (1) {
		char buf[256];
		int size = _read(fd, buf, sizeof(buf) - 1);
		if (size <= 0)
			break;
		_write(1, buf, size);
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
