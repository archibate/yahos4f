#include <string.h>

int strcmp(const char *dst, const char *src)
{
	const unsigned char *p = dst, *q = src;
	int ret = 0;
	while (!(ret = *p - *q) && *p)
		++p, ++q;
	return ret < 0 ? -1 : !!ret;
}
