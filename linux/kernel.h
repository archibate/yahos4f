#pragma once

#include <stdarg.h>

void syslog(const char *msg);
void printk(const char *msg, ...);
void vprintk(const char *msg, va_list ap);
void __attribute__((noreturn)) panic(const char *msg, ...);
void __attribute__((noreturn)) vpanic(const char *msg, va_list ap);
void __attribute__((noreturn)) fail(const char *msg);

#define static_assert(x) switch (0) { case (x): case 0: break; }
