#pragma once

void __attribute__((noreturn, fastcall)) move_to_user(void *sp, unsigned int eflags);
