#include <string.h>

int memcmp(const void *dst, const void *src, size_t size)
{
	const unsigned char *p = dst, *q = src;
	int ret = 0;
	while (size-- && !(ret = *p - *q))
		++p, ++q;
	return ret < 0 ? -1 : !!ret;
}
