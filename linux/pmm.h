#pragma once

#include <linux/mmu.h>
#include <linux/segment.h>

#define NR_PMM_PAGES	256

void *alloc_page(void);
void free_page(void *page);
void init_pmm(void);
