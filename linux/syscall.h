#pragma once

#ifndef __user
#define __user
#endif

#ifdef _SYSCALL_KERNEL_DEFINATION

#define _syscallv(i, rt, name, t1) \
	case (i): sys_##name((t1) ebx); break;
#define _syscall0(i, rt, name) \
	case (i): *&eax = sys_##name(); break;
#define _syscall1(i, rt, name, t1) \
	case (i): *&eax = sys_##name((t1) ebx); break;
#define _syscall2(i, rt, name, t1, t2) \
	case (i): *&eax = sys_##name((t1) ebx, (t2) ecx); break;
#define _syscall3(i, rt, name, t1, t2, t3) \
	case (i): *&eax = sys_##name((t1) ebx, (t2) ecx, (t3) edx); break;
#define _syscall4(i, rt, name, t1, t2, t3, t4) \
	case (i): *&eax = sys_##name((t1) ebx, (t2) ecx, (t3) edx, \
				  (t4) esi); break;
#define _syscall5(i, rt, name, t1, t2, t3, t4, t5) \
	case (i): *&eax = sys_##name((t1) ebx, (t2) ecx, (t3) edx, \
				  (t4) esi, (t5) edi); break;

#else

#ifndef _SYSCALL
#define _SYSCALL
#endif

#ifdef _DEFINE_SYSCALL
#define _DEF_SYS(x) x
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
	_SYSCALL rt _##name(void) \
	_DEF_SYS({ \
		rt ret; \
		asm volatile ("int $0x80" \
				: "=a" (ret) : "a" (i)); \
		return ret; \
	})
#define _syscall1(i, rt, name, t1) \
	_SYSCALL rt _##name(t1 x1) \
	_DEF_SYS({ \
		rt ret; \
		asm volatile ("int $0x80" \
				: "=a" (ret) : "a" (i), "b" (x1)); \
		return ret; \
	})
#define _syscall2(i, rt, name, t1, t2) \
	_SYSCALL rt _##name(t1 x1, t2 x2) \
	_DEF_SYS({ \
		rt ret; \
		asm volatile ("int $0x80" \
				: "=a" (ret) : "a" (i), "b" (x1), "c" (x2)); \
		return ret; \
	})
#define _syscall3(i, rt, name, t1, t2, t3) \
	_SYSCALL rt _##name(t1 x1, t2 x2, t3 x3) \
	_DEF_SYS({ \
		rt ret; \
		asm volatile ("int $0x80" \
				: "=a" (ret) : "a" (i), "b" (x1) \
				, "c" (x2), "d" (x3)); \
		return ret; \
	})
#define _syscall4(i, rt, name, t1, t2, t3, t4) \
	_SYSCALL rt _##name(t1 x1, t2 x2, t3 x3, t4 x4) \
	_DEF_SYS({ \
		rt ret; \
		asm volatile ("int $0x80" \
				: "=a" (ret) : "a" (i), "b" (x1) \
				, "c" (x2), "d" (x3), "S" (x4)); \
		return ret; \
	})
#define _syscall5(i, rt, name, t1, t2, t3, t4, t5) \
	_SYSCALL rt _##name(t1 x1, t2 x2, t3 x3, t4 x4, t5 x5) \
	_DEF_SYS({ \
		rt ret; \
		asm volatile ("int $0x80" \
				: "=a" (ret) : "a" (i), "b" (x1) \
				, "c" (x2), "d" (x3), "S" (x4), "D" (x5)); \
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