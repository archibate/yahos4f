#include <string.h>
#include <malloc.h>

char *strdup(const char *s)
{
	char *p = malloc(strlen(s) + 1);
	strcpy(p, s);
	return p;
}
