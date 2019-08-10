#include <linux/kernel.h>

void __attribute__((noreturn)) __stack_chk_fail(void)
{
	panic("__stack_chk_fail() called");
}

void __attribute__((noreturn)) __stack_chk_fail_local(void)
{
	panic("__stack_chk_fail_local() called");
}
