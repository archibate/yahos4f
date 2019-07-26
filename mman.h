#pragma once

void init_mman(void);
void *malloc(unsigned long size);
unsigned long mcheck(void *p);
void free(void *p);
