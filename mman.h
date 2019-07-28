#pragma once

void init_mman(void);
void *sbrk(long incptr);
void *brk(void *newp);

void *malloc(unsigned long size);
void *realloc(void *p, unsigned long size); // (WIP)
void *calloc(unsigned long nmemb, unsigned long size);
void free(void *p);
