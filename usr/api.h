#pragma once

/* keep sync with syscall.c */

static int getchar(void)
{
	int r;
	asm volatile ("int $0x80" : "=a" (r) : "a" (4));
	return r;
}

static void cputs(unsigned int color, const char *s)
{
	asm volatile ("int $0x80" :: "a" (1), "c" (color), "d" (s));
}

static void pause(void)
{
	asm volatile ("int $0x80" :: "a" (2));
}

static void wfi(void)
{
	asm volatile ("int $0x80" :: "a" (3));
}

static void rdblk(int dev, int blkno, void *buf)
{
	asm volatile ("int $0x80" :: "a" (5), "c" (dev), "d" (blkno), "b" (buf));
}

static void wrblk(int dev, int blkno, const void *buf)
{
	asm volatile ("int $0x80" :: "a" (6), "c" (dev), "d" (blkno), "b" (buf));
}
