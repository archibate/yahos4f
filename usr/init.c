#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv)
{
	chdir("/root");

	open("/dev/tty", O_RDONLY);
	open("/dev/tty", O_WRONLY);
	dup(1);

	write(1, "init started\n", 13);
	execlp("sh", "/etc/rcS", NULL);
	write(1, "init error\n", 11);

	return 0;
}
