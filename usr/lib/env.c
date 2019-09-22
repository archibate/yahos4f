#include <stdlib.h>

extern char **__envp;

char *getenv(const char *name)
{
	for (char **envp = __envp; *envp; envp++) {
		char *s = *envp;
		while (*s && *s != '=') s++;
		return s;
	}
	return NULL;
}
