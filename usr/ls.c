#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

extern int dirread(int dd, struct dirent *de);

void lsfile(int dd, const char *parent, const char *name)
{
	char buf[23];
	struct stat st;
	if (fstatat(dd, name, &st, 0) == -1) {
		if (parent) {
			write(2, parent, strlen(parent));
			write(2, "/", 1);
		}
		write(2, name, strlen(name));
		write(2, ": stat error\n", 13);
		return;
	}
	sprintf(buf, "%5d ", st.st_ino);
	write(2, buf, strlen(buf));
	write(2, name, strlen(name));
	if (S_ISDIR(st.st_mode))
		write(2, "/", 1);
	write(2, "\n", 1);
}

void ls(const char *path)
{
	int dd = open(path, O_RDONLY | O_DIRECTORY);
	if (dd == -1) {
		lsfile(AT_FDCWD, NULL, path);
		return;
	}
	struct dirent de;
	while (dirread(dd, &de) == 1) {
		lsfile(dd, path, de.d_name);
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
