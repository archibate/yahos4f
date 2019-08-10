#include <linux/mman.h>

static void *mptr = (void *)0x200000;

void init_mman(void)
{
}

void *sbrk(long incptr)
{
	void *p = mptr;
	mptr += incptr;
	return p;
}

void *brk(void *newp)
{
	void *p = mptr;
	mptr = newp;
	return p;
}
