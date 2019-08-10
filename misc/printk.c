#include <linux/kernel.h>
#include <linux/conio.h>
#include <stdio.h>
#include <stdarg.h>

void __vprintk(_ARG_SINFO, const char *fmt, va_list ap)
{
	static char buf[2048];
	vsprintf(buf, fmt, ap);
	if (_HAS_ARG_SINFO)
		cprintf("%s:%d: %s: %s\n", _PASS_SINFO, buf);
	else
		cprintf("%s\n", buf);
}

void __printk(_ARG_SINFO, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	__vprintk(_PASS_SINFO, fmt, ap);
	va_end(ap);
}

void __attribute__((noreturn)) __vpanic(_ARG_SINFO, const char *fmt, va_list ap)
{
	static char buf[2048];
	int ret = vsprintf(buf, fmt, ap);
	__printk(_PASS_SINFO, "%s", buf);
	for (;;) asm volatile ("cli\nhlt");
}

void __attribute__((noreturn)) __panic(_ARG_SINFO, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	__vpanic(_PASS_SINFO, fmt, ap);
}
