#pragma once

#include <linux/cli.h>

typedef struct atomic {
	int i;
} atomic_t;

static int atomic_get(atomic_t *a)
{
	cli();
	int ret = a->i;
	sti();
	return ret;
}

static int atomic_set(atomic_t *a, int n)
{
	cli();
	int ret = a->i;
	a->i = n;
	sti();
	return ret;
}

static int atomic_setifz(atomic_t *a, int n)
{
	cli();
	int ret = a->i;
	if (!ret) a->i = n;
	sti();
	return ret;
}

static int atomic_add(atomic_t *a, int n)
{
	cli();
	int ret = a->i + n;
	a->i = ret;
	sti();
	return ret;
}

static int atomic_sub(atomic_t *a, int n)
{
	cli();
	int ret = a->i - n;
	a->i = ret;
	sti();
	return ret;
}

static int atomic_subu(atomic_t *a, int n)
{
	cli();
	int ret = a->i - n;
	if (ret >= 0)
		a->i = ret;
	sti();
	return ret;
}
