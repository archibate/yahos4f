#include <linux/kernel.h>
#include <linux/vmm.h>
#include <linux/pmm.h>

void *page_stack[NR_PMM_PAGES], **psp;

void *alloc_page(void)
{
	if (psp <= page_stack)
		panic("out of memory");
	return *--psp;
}

void free_page(void *page)
{
	if (psp >= page_stack + NR_PMM_PAGES)
		panic("should not be too much free");
	*psp++ = page;
}

void init_pmm(void)
{
	void *p0 = (void *)0x300000;
	psp = page_stack;
	for (int i = 0; i < NR_PMM_PAGES; i++) {
		free_page(p0 + i * PGSIZE);
	}
}
