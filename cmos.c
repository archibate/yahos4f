// https://github.com/archibate/Linux011/blob/master/src/V0.11/init/main.c
#include "time.h"
#include "io.h"

#define IO_CMOS	0x70

static int cmos_read(int i)
{
	outb(IO_CMOS+0, 0x80 | i);
	int bcd = inb(IO_CMOS+1);
	return (bcd & 15) + (bcd >> 4) * 10;
}

void cmos_gettime(struct tm *t)
{
	t->tm_sec = cmos_read(0);
	t->tm_min = cmos_read(2);
	t->tm_hour = cmos_read(4);
	t->tm_mday = cmos_read(7);
	t->tm_mon = cmos_read(8) - 1;
	t->tm_year = cmos_read(9);
}
