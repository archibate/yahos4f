#pragma once

void __attribute__((noreturn)) move_to_user(
		unsigned long pc, unsigned long sp, unsigned int eflags);
