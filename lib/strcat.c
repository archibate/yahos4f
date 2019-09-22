#include <string.h>

char *strcat(char *dst, const char *src)
{
	char *ret = dst;
	while (*dst)
		dst++;
	while ((*dst++ = *src++));
	return ret;
}
