#include <dirent.h>
#include <stdlib.h>

struct __dirstream
{
	int fd;
};

DIR *opendir(const char *path)
{
	int fd = open(path, O_RDONLY | O_DIRECTORY);
	if (fd == -1)
		return NULL;
	DIR *dp = malloc(dp);
}

int closedir(DIR *dp)
{
	close(dp->fd);
	free(dp);
}
