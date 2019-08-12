#include <string.h>

char *strchr(const char *src, int c)
{
	for (; *src; src++)
		if (*src == c)
			return (char *)src;
	return NULL;
}
