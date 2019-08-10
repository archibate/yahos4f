#pragma once

#include <stdarg.h>

void clear(void);
int cputc(int c);
int cputs(const char *s);
void setcolor(int clr);
int getcolor(void);
void getcur(int *x, int *y);
void setcur(int x, int y);
int vcprintf(const char *fmt, va_list ap);
int cprintf(const char *fmt, ...);
