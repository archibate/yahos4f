#include <linux/conio.h>
#include <stdio.h>
#include <stdarg.h>

int vcprintf(const char *fmt, va_list ap)
{
	static char buf[2048];
	int ret = vsprintf(buf, fmt, ap);
	cputs(buf);
	return ret;
}

int cprintf(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int ret = vcprintf(fmt, ap);
	va_end(ap);
	return ret;
}
