#include <stdlib.h>

int atoi(const char *s)
{
	int r = 0;
	while ('0' <= *s && *s <= '9')
		r = r * 10 + *s++ - '0';
	return r;
}
