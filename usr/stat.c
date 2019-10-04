#include <unistd.h>
#include <sys/stat.h>

void stat(const char *path)
{
	struct stat st;
	fstatat(AT_FDCWD, de.d_name, &st, AT_SYMLINK_NOFOLLOW);
	if (S_ISDIR(st.st_mode))
		write(2, "<DIR> ", 6);
	else if (S_ISCHR(st.st_mode))
		write(2, "<CHR> ", 6);
	else if (S_ISBLK(st.st_mode))
		write(2, "<BLK> ", 6);
	else	write(2, "      ", 6);
	TODO!;
}

int main(int argc, char *const *argv)
{
	if (argc == 1) {
		write(2, "stat: missing arguments\n", 24);
		return -1;
	}
	else for (int i = 1; i < argc; i++)
		stat(argv[i]);
	return 0;
}
