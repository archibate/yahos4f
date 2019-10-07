#include <stdio.h>
#include <stdarg.h>

int sprintf(char *buf, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int ret = vsprintf(buf, fmt, ap);
	va_end(ap);
	return ret;
}
