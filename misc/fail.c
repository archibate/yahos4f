#include <linux/kernel.h>

void __attribute__((noreturn)) fail(const char *msg)
{
	printk("FAIL: %s", msg);
	for (;;)
		asm volatile ("cli\nhlt");
}

void __attribute__((noreturn)) __stack_chk_fail(void)
{
	fail("__stack_chk_fail() called");
}

void __attribute__((noreturn)) __stack_chk_fail_local(void)
{
	fail("__stack_chk_fail_local() called");
}
