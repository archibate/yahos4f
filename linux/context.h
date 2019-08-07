#pragma once

struct cont
{
	unsigned long sp;
	unsigned long bx, si, di, bp;
	unsigned long eflags;
} __attribute__((packed));

void __attribute__((fastcall)) switch_context(struct cont *prev, struct cont *next);
