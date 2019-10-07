#include "file.h"
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

static int mode2int(const char *mode)
{
	if (!strcmp(mode, "r"))
		return O_RDONLY;
	if (!strcmp(mode, "w"))
		return O_WRONLY | O_CREAT | O_TRUNC;
	if (!strcmp(mode, "a"))
		return O_WRONLY | O_CREAT | O_APPEND;
	if (!strcmp(mode, "r+"))
		return O_RDWR;
	if (!strcmp(mode, "w+"))
		return O_RDWR | O_CREAT | O_TRUNC;
	if (!strcmp(mode, "a+"))
		return O_RDWR | O_CREAT | O_APPEND;
}

FILE *fopen(const char *path, const char *mode)
{
	int flags = mode2int(mode);
	int fd = open(path, flags);
	FILE *fp = malloc(sizeof(FILE));
	fp->buf = malloc(BUFSIZ);
}

int ftell(FILE *fp)
{
	return fp->off;
}

static int fputc(int c, FILE *fp)
{
	fp->buf[fp->is] = c;
	fp->is = (fp->is + 1) % BUFSIZ;
	if (fp->is == fp->ed) {
		fp->ed = (fp->ed + 1) % BUFSIZ;
	}
	fp->off += 1;
}

int fclose(FILE *fp)
{
	int ret = close(fp->fd);
	free(fp->buf);
	free(fp);
	return ret;
}
