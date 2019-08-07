#pragma once

/* keep sync with syscall.c */

static int _exit(void)
{
	asm volatile ("int $0x80" :: "a" (1));
}

static int pause(void)
{
	asm volatile ("int $0x80" :: "a" (2));
}

static int getpid(void)
{
	int r;
	asm volatile ("int $0x80" : "=a" (r) : "a" (3));
	return r;
}

static int getppid(void)
{
	int r;
	asm volatile ("int $0x80" : "=a" (r) : "a" (4));
	return r;
}
