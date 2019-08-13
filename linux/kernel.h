#pragma once

#include <stdarg.h>

#define _PASS_SINFO	_file, _line, _func
#define _ARG_SINFO	const char *_file, int _line, const char *_func
#define _SPEC_SINFO	__FILE__, __LINE__, __func__
#define _NO_SINFO	0, 0, 0
#define _HAS_ARG_SINFO	(!!_file)

#define KL_DEBUG	"DEBUG: "
#define KL_INFO		"INFO: "
#define KL_WARN		"WARNING: "
#define KL_ERROR	"ERROR: "
#define KL_PANIC	"PANIC: "

void __printk(_ARG_SINFO, const char *fmt, ...);
void __vprintk(_ARG_SINFO, const char *fmt, va_list ap);
void __attribute__((noreturn)) __panic(_ARG_SINFO, const char *fmt, ...);
void __attribute__((noreturn)) __vpanic(_ARG_SINFO, const char *fmt, va_list ap);
#define printk(...)	__printk(_NO_SINFO, __VA_ARGS__)
#define warning(...)	__printk(KL_WARN  _SPEC_SINFO, __VA_ARGS__)
#define inform(...)	__printk(KL_INFO  _SPEC_SINFO, __VA_ARGS__)
#define debug(...)	__printk(KL_DEBUG _SPEC_SINFO, __VA_ARGS__)
#define panic(...)	__panic( KL_PANIC _SPEC_SINFO, __VA_ARGS__)
#define show(var, fmt)	debug(#var "=" #fmt, var)

#define static_assert(x) switch (0) { case (x): case 0: break; }

static int ffasqaf(void)
{
}
