#pragma once

<<<<<<< HEAD
#define __user

static inline unsigned char get_fs_byte(const unsigned char __user *addr)
=======
static inline unsigned char get_fs_byte(const unsigned char *addr)
>>>>>>> 9ece444c0c26779e89ded70225ba21578dd28eda
{
	unsigned register char val;
	asm volatile ("movb %%fs:%1, %0" : "=r" (val) : "m" (*addr));
	return val;
}

<<<<<<< HEAD
static inline unsigned short get_fs_word(const unsigned short __user *addr)
=======
static inline unsigned short get_fs_word(const unsigned short *addr)
>>>>>>> 9ece444c0c26779e89ded70225ba21578dd28eda
{
	unsigned short val;
	asm volatile ("movw %%fs:%1, %0" : "=r" (val) : "m" (*addr));
	return val;
}

<<<<<<< HEAD
static inline unsigned long get_fs_long(const unsigned long __user *addr)
=======
static inline unsigned long get_fs_long(const unsigned long *addr)
>>>>>>> 9ece444c0c26779e89ded70225ba21578dd28eda
{
	unsigned long val;
	asm volatile ("movl %%fs:%1, %0" : "=r" (val) : "m" (*addr));
	return val;
}

<<<<<<< HEAD
static inline void put_fs_byte(char val, unsigned char __user *addr)
=======
static inline void put_fs_byte(char val, unsigned char *addr)
>>>>>>> 9ece444c0c26779e89ded70225ba21578dd28eda
{
	asm volatile ("movb %0, %%fs:%1" :: "r" (val), "m" (*addr));
}

<<<<<<< HEAD
static inline void put_fs_word(short val, unsigned short __user *addr)
=======
static inline void put_fs_word(short val, unsigned short *addr)
>>>>>>> 9ece444c0c26779e89ded70225ba21578dd28eda
{
	asm volatile ("movw %0, %%fs:%1" :: "r" (val), "m" (*addr));
}

<<<<<<< HEAD
static inline void put_fs_long(unsigned long val, unsigned long __user *addr)
{
	asm volatile ("movl %0, %%fs:%1" :: "r" (val), "m" (*addr));
}

static inline void write_fs_memory(void __user *dst, const void *src, size_t size)
{
	asm volatile (
			"push %%es\n"
			"push %%fs\n"
			"pop %%es\n"
			"cld;rep;movsb\n"
			"pop %%es\n"
			:: "D" (dst), "S" (src), "c" (size));
}

static inline void read_fs_memory(void *dst, const void __user *src, size_t size)
{
	asm volatile (
			"push %%ds\n"
			"push %%fs\n"
			"pop %%ds\n"
			"cld;rep;movsb\n"
			"pop %%ds\n"
			:: "D" (dst), "S" (src), "c" (size));
}

static inline void zero_fs_memory(void __user *dst, size_t size)
{
	asm volatile (
			"push %%es\n"
			"push %%fs\n"
			"pop %%es\n"
			"cld;rep;stosb\n"
			"pop %%es\n"
			:: "D" (dst), "a" (0));
}
=======
static inline void put_fs_long(unsigned long val, unsigned long *addr)
{
	asm volatile ("movl %0, %%fs:%1" :: "r" (val), "m" (*addr));
}
>>>>>>> 9ece444c0c26779e89ded70225ba21578dd28eda
