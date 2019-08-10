#include <linux/kernel.h>
#include <stdio.h>
#include <stdarg.h>

void vprintk(const char *fmt, va_list ap)
{
	static char buf[2048];
	vsprintf(buf, fmt, ap);
	syslog(buf);
}

void printk(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vprintk(fmt, ap);
	va_end(ap);
}

void __attribute__((noreturn)) vpanic(const char *fmt, va_list ap)
{
	static char buf[2048];
	int ret = vsprintf(buf, fmt, ap);
	fail(buf);
}

void __attribute__((noreturn)) panic(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vpanic(fmt, ap);
}
