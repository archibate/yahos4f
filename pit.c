#include "pit.h"
#include "io.h"

// https://github.com/archibate/OS67/blob/master/kern/timer.c
#define IO_PIT_CNTL  0x43
#define IO_PIT_CTR0  0x40

void set_timer_freq(int hz)
{
	int ctr = 1193180 / hz;

	outb(IO_PIT_CNTL, 0x34);
	outb(IO_PIT_CTR0, ctr & 0xff);
	outb(IO_PIT_CTR0, ctr >> 8);
}
