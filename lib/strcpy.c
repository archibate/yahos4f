#include <string.h>

char *strcpy(char *dst, const char *src)
{
	char *ret = dst;
	while ((*dst++ = *src++));
	return ret;
}
