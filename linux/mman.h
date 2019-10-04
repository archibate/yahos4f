#pragma once

void init_mman(void);
void *sbrk(long incptr);
void *brk(void *newp);

void *malloc(unsigned long size);
void *calloc(unsigned long nmemb, unsigned long size);
void free(void *p);
