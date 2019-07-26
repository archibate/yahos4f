#include "mman.h"
#include "console.h"

#define GAP1_MAGIC	0xfefdfced
#define GAP2_MAGIC	0xfdfeeffd

static void *mptr = (void *)0x200000;

void init_mman(void)
{
}

void *malloc(unsigned long size)
{
	unsigned long *up = mptr;
	*up++ = GAP1_MAGIC;
	*up++ = size;
	void *p = up;
	up = p + size;
	*up++ = GAP2_MAGIC;
	mptr = up;
	for (char *q = p; (void *)q < p + size; q++)
		*q = 0;
	return p;
}

unsigned long mcheck(void *p)
{
	unsigned long *up = p;
	unsigned long size = up[-1];
	if (up[-2] != GAP1_MAGIC) {
		puts("mcheck: Gap 1 Broken\n");
		return 0;
	}
	up = p + size;
	if (up[0] != GAP2_MAGIC) {
		puts("mcheck: Gap 2 Broken\n");
		return 0;
	}
	return size;
}

void free(void *p)
{
	unsigned long size = mcheck(p);
	for (char *q = p; (void *)q < p + size; q++)
		*q = 0xcc;
}
