#pragma once

void init_mman(void);
void *sbrk(long incptr);
void *brk(void *newp);
