#include <linux/syscall.h>
#include <fcntl.h>
#include <string.h>

char *fdgets(int fd, char *buf, size_t size)
{
	char ch = 0;
	if (!size--)
		return NULL;
	while (size) {
		_read(fd, &ch, 1);
		*buf++ = ch;
		if (ch == '\n')
			break;
		size--;
	}
	*buf++ = 0;
	return buf;
}

int main(void)
{
	_open("/dev/tty", O_RDONLY);
	_open("/dev/tty", O_WRONLY);
	_dup(1);

	while (1) {
		_write(1, "> ", 2);
		char buf[256];
		fdgets(0, buf, sizeof(buf));
		_write(1, "Your input: ", 12);
		_write(1, buf, strlen(buf));
	}

	return 0;
}
