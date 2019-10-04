#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

extern int dirread(int dd, struct dirent *de);

void ls(const char *path)
{
	int dd = open(path, O_RDONLY | O_DIRECTORY);
	if (dd == -1) {
		write(2, path, strlen(path));
		write(2, ": open error\n", 13);
		return;
	}
	struct dirent de;
	while (dirread(dd, &de) == 1) {
		write(2, de.d_name, strlen(de.d_name));
		write(2, "\n", 1);
	}
	close(dd);
}

int main(int argc, char *const *argv)
{
	if (argc == 1) ls(".");
	else for (int i = 1; i < argc; i++)
		ls(argv[i]);
	return 0;
}
