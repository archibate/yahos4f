#pragma once

#include <sys/types.h>
#include <sys/types.h>

#ifndef __user
#define __user
#endif

#ifndef _SYSCALL
#define _SYSCALL
#endif

#ifdef _SYSCALL_KERNEL_DEFINATION
	errno = 0;
	switch (eax) {
#define _syscallv(i, rt, name, t1) \
	_SYSCALL rt __attribute__((noreturn)) sys_##name(t1 x1); \
	case (i): sys_##name((t1) ebx); break;
#define _syscall0(i, rt, name) \
	_SYSCALL rt sys_##name(void); \
	case (i): *&eax = (long)sys_##name(); *&ecx = errno; break;
#define _syscall1(i, rt, name, t1) \
	_SYSCALL rt sys_##name(t1); \
	case (i): *&eax = (long)sys_##name((t1) ebx); *&ecx = errno; break;
#define _syscall2(i, rt, name, t1, t2) \
	_SYSCALL rt sys_##name(t1, t2); \
	case (i): *&eax = (long)sys_##name((t1) ebx, (t2) ecx); \
		  *&ecx = errno; break;
#define _syscall3(i, rt, name, t1, t2, t3) \
	_SYSCALL rt sys_##name(t1, t2, t3); \
	case (i): *&eax = (long)sys_##name((t1) ebx, (t2) ecx, (t3) edx); \
		  *&ecx = errno; break;
#define _syscall4(i, rt, name, t1, t2, t3, t4) \
	_SYSCALL rt sys_##name(t1, t2, t3, t4); \
	case (i): *&eax = (long)sys_##name((t1) ebx, (t2) ecx, (t3) edx, \
				  (t4) esi); *&ecx = errno; break;
#define _syscall5(i, rt, name, t1, t2, t3, t4, t5) \
	_SYSCALL rt sys_##name(t1, t2, t3, t4, t5); \
	case (i): *&eax = (long)sys_##name((t1) ebx, (t2) ecx, (t3) edx, \
				  (t4) esi, (t5) edi); *&ecx = errno; break;

#else

#ifdef _DEFINE_SYSCALL
#define _DEF_SYS(x) x
#include <errno.h>
#else
#define _DEF_SYS(x) ;
#endif

#define _syscallv(i, rt, name, t1) \
	_SYSCALL rt __attribute__((noreturn)) _##name(t1 x1) \
	_DEF_SYS({ \
		asm volatile ("int $0x80" \
				:: "a" (i), "b" (x1)); \
		for (;;) asm volatile ("ud2"); \
	})
#define _syscall0(i, rt, name) \
	_SYSCALL rt name(void) \
	_DEF_SYS({ \
		rt ret; \
		int errnum; \
		asm volatile ("int $0x80" \
				: "=a" (ret), "=c" (errnum) : "a" (i)); \
		return ret; \
	})
#define _syscall1(i, rt, name, t1) \
	_SYSCALL rt name(t1 x1) \
	_DEF_SYS({ \
		rt ret; \
		int errnum; \
		asm volatile ("int $0x80" \
				: "=a" (ret), "=c" (errnum) : "a" (i) \
				, "b" (x1)); \
		if (errnum) errno = errnum; \
		return ret; \
	})
#define _syscall2(i, rt, name, t1, t2) \
	_SYSCALL rt name(t1 x1, t2 x2) \
	_DEF_SYS({ \
		rt ret; \
		int errnum; \
		asm volatile ("int $0x80" \
				: "=a" (ret), "=c" (errnum) : "a" (i) \
				, "b" (x1), "c" (x2)); \
		if (errnum) errno = errnum; \
		return ret; \
	})
#define _syscall3(i, rt, name, t1, t2, t3) \
	_SYSCALL rt name(t1 x1, t2 x2, t3 x3) \
	_DEF_SYS({ \
		rt ret; \
		int errnum; \
		asm volatile ("int $0x80" \
				: "=a" (ret), "=c" (errnum) : "a" (i) \
				, "b" (x1) , "c" (x2), "d" (x3)); \
		if (errnum) errno = errnum; \
		return ret; \
	})
#define _syscall4(i, rt, name, t1, t2, t3, t4) \
	_SYSCALL rt name(t1 x1, t2 x2, t3 x3, t4 x4) \
	_DEF_SYS({ \
		rt ret; \
		int errnum; \
		asm volatile ("int $0x80" \
				: "=a" (ret), "=c" (errnum) : "a" (i) \
				, "b" (x1) , "c" (x2), "d" (x3), "S" (x4)); \
		if (errnum) errno = errnum; \
		return ret; \
	})
#define _syscall5(i, rt, name, t1, t2, t3, t4, t5) \
	_SYSCALL rt name(t1 x1, t2 x2, t3 x3, t4 x4, t5 x5) \
	_DEF_SYS({ \
		rt ret; \
		int errnum; \
		asm volatile ("int $0x80" \
				: "=a" (ret), "=c" (errnum) : "a" (i) \
				, "b" (x1) , "c" (x2), "d" (x3), "S" (x4) \
				, "D" (x5)); \
		if (errnum) errno = errnum; \
		return ret; \
	})
#endif

_syscallv(1, void, exit, int);
_syscall0(2, int, pause);
_syscall0(3, int, getpid);
_syscall0(4, int, getppid);
_syscall1(5, int, debug, const char __user *);
_syscall1(6, int, rmdir, const char __user *);
_syscall2(7, int, mkdir, const char __user *, unsigned int);
_syscall2(8, int, link, const char __user *, const char __user *);
_syscall1(9, int, unlink, const char __user *);
_syscall3(10, int, mknod, const char __user *, unsigned int, unsigned int);
_syscall3(11, int, open, const char __user *, int, int);
_syscall3(12, int, write, int, const void __user *, size_t);
_syscall3(13, int, read, int, void __user *, size_t);
_syscall3(14, off_t, lseek, int, off_t, int);
_syscall2(15, int, dup2, int, int);
_syscall1(16, int, dup, int);
_syscall1(17, int, close, int);
_syscall3(18, int, execve, const char __user *, char __user *const __user *,
		char __user *const __user *);
_syscall1(19, int, chdir, const char __user *);
_syscall0(20, int, fork);
_syscall1(21, int, wait, int __user *);
struct dirent;
_syscall2(22, int, dirread, int, struct dirent __user *);
_syscall1(23, int, dirrewind, int);
struct stat;
_syscall4(24, int, fstatat, int, const char __user *, struct stat __user *, int);
_syscall1(25, int, brk, void __user *);
_syscall1(26, void __user *, sbrk, int);
_syscall1(27, int, reboot, int);
_syscall0(28, int, sync);

#ifdef _SYSCALL_KERNEL_DEFINATION
	default:
		warning("undefined syscall %d (%#x)", eax);
		*&eax = -1;
		*&ecx = -ENOSYS;
	}
#endif
