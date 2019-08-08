#pragma once

static void cli(void)
{
	asm volatile ("cli");
}

static void sti(void)
{
	asm volatile ("sti");
}
