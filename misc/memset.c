#include <string.h>

void *memset(void *dst, int val, size_t size)
{
	char *p = dst, v = val;
	while (size--)
		*p++ = v;
	return dst;
}
