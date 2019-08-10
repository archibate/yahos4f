#include <linux/kernel.h>
#include <linux/console.h>

void syslog(const char *s)
{
	cputs(s);
	cputc('\n');
}
