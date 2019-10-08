#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

int ls_show_all = 0;

extern int dirread(int dd, struct dirent *de); // my system api da!

void lsfile(int dd, const char *parent, const char *name)
{
	char buf[23];
	struct stat st;
	if (fstatat(dd, name, &st, 0) == -1) {
		if (parent) {
			printf("%s/", parent);
		}
		perror(name);
		return;
	}
	printf("%5d %s", st.st_ino, name);
	if (S_ISDIR(st.st_mode))
		printf("/");
	printf("\n");
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
		if (de.d_name[0] == '.') {
			if (!ls_show_all)
				continue;
			else if (ls_show_all == 1) { // -A
				switch (de.d_name[1]) {
				case '.': case 0:
					continue;
				}
			}
		}
		lsfile(dd, path, de.d_name);
	}
	close(dd);
}

int main(int argc, char *const *argv)
{
	argv++, argc--;
	if (argc && argv[0][0] == '-') {
		switch (argv[0][1]) {
		case 'a': ls_show_all = 2; break;
		case 'A': ls_show_all = 1; break;
		}
		argv++, argc--;
	}
	if (!argc) ls(".");
	else if (argc == 1) ls(argv[0]);
	else for (int i = 0; i < argc; i++) {
		printf("%s:\n", argv[i]);
		ls(argv[i]);
		if (i != argc - 1)
			printf("\n", argv[i]);
	}
	return 0;
}
