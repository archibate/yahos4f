#pragma once

#include <stddef.h>

#ifndef __user
#define __user
#endif

static inline unsigned char get_fs_byte(const unsigned char __user *addr)
{
	unsigned register char val;
	asm volatile ("movb %%fs:%1, %0" : "=r" (val) : "m" (*addr));
	return val;
}

static inline unsigned short get_fs_word(const unsigned short __user *addr)
{
	unsigned short val;
	asm volatile ("movw %%fs:%1, %0" : "=r" (val) : "m" (*addr));
	return val;
}

static inline unsigned long get_fs_long(const unsigned long __user *addr)
{
	unsigned long val;
	asm volatile ("movl %%fs:%1, %0" : "=r" (val) : "m" (*addr));
	return val;
}

static inline void put_fs_byte(char val, unsigned char __user *addr)
{
	asm volatile ("movb %0, %%fs:%1" :: "r" (val), "m" (*addr));
}

static inline void put_fs_word(short val, unsigned short __user *addr)
{
	asm volatile ("movw %0, %%fs:%1" :: "r" (val), "m" (*addr));
}

static inline void put_fs_long(unsigned long val, unsigned long __user *addr)
{
	asm volatile ("movl %0, %%fs:%1" :: "r" (val), "m" (*addr));
}

static inline void copy_to_user(void __user *dst, const void *src, size_t size)
{
	asm volatile (
			"push %%es\n"
			"push %%fs\n"
			"pop %%es\n"
			"cld;rep;movsb\n"
			"pop %%es\n"
			:: "D" (dst), "S" (src), "c" (size));
}

static inline void copy_from_user(void *dst, const void __user *src, size_t size)
{
	asm volatile (
			"push %%ds\n"
			"push %%fs\n"
			"pop %%ds\n"
			"cld;rep;movsb\n"
			"pop %%ds\n"
			:: "D" (dst), "S" (src), "c" (size));
}

static inline void clear_user(void __user *dst, size_t size)
{
	asm volatile (
			"push %%es\n"
			"push %%fs\n"
			"pop %%es\n"
			"cld;rep;stosb\n"
			"pop %%es\n"
			:: "D" (dst), "a" (0));
}
