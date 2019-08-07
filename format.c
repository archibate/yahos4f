#include <string.h>
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

int vprintf(const char *fmt, va_list ap)
{
	static char buf[2048];
	int ret = vsprintf(buf, fmt, ap);
	puts(buf);
	return ret;
}

int printf(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int ret = vprintf(fmt, ap);
	va_end(ap);
	return ret;
}
