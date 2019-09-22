#include <linux/syscall.h>
#include <string.h>

int main(int argc, char **argv)
{
	for (int i = 1; i < argc; i++) {
		if (i != 1) _write(1, " ", 1);
		_write(1, argv[i], strlen(argv[i]));
	}
	_write(1, "\n", 1);
}
