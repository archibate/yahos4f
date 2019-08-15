#pragma once

static inline unsigned char get_fs_byte(const unsigned char *addr)
{
	unsigned register char val;
	asm volatile ("movb %%fs:%1, %0" : "=r" (val) : "m" (*addr));
	return val;
}

static inline unsigned short get_fs_word(const unsigned short *addr)
{
	unsigned short val;
	asm volatile ("movw %%fs:%1, %0" : "=r" (val) : "m" (*addr));
	return val;
}

static inline unsigned long get_fs_long(const unsigned long *addr)
{
	unsigned long val;
	asm volatile ("movl %%fs:%1, %0" : "=r" (val) : "m" (*addr));
	return val;
}

static inline void put_fs_byte(char val, unsigned char *addr)
{
	asm volatile ("movb %0, %%fs:%1" :: "r" (val), "m" (*addr));
}

static inline void put_fs_word(short val, unsigned short *addr)
{
	asm volatile ("movw %0, %%fs:%1" :: "r" (val), "m" (*addr));
}

static inline void put_fs_long(unsigned long val, unsigned long *addr)
{
	asm volatile ("movl %0, %%fs:%1" :: "r" (val), "m" (*addr));
}
