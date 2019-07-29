#pragma once

int getchar(void)
{
	int r;
	asm volatile ("int $0x80" : "=a" (r) : "a" (4));
	return r;
}

void cputs(unsigned int color, const char *s)
{
	asm volatile ("int $0x80" :: "a" (1), "c" (color), "d" (msg));
}

void pause(void)
{
	asm volatile ("int $0x80" :: "a" (2));
}

void wfi(void)
{
	asm volatile ("int $0x80" :: "a" (3));
}

void ide_rdblk(dev_t dev, blkno_t blkno, void *buf)
{
	asm volatile ("int $0x80" :: "a" (5), "c" (dev), "d" (blkno), "b" (buf));
}

void ide_wrblk(unsigned dev, unsigned blkno, const void *buf)
{
	asm volatile ("int $0x80" :: "a" (6), "c" (dev), "d" (blkno), "b" (buf));
}
