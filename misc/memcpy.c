#include <string.h>

void *memcpy(void *dst, const void *src, size_t size)
{
	char *p = dst;
	const char *q = src;
	while (size--)
		*p++ = *q++;
	return dst;
}
