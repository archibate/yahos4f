#include <linux/kernel.h>
#include <linux/mmu.h>
#include <string.h>

static unsigned long __attribute__((aligned(4096))) boot_pts[BOOT_PTS_NR][1024];
static unsigned long __attribute__((aligned(4096))) boot_pd[1024];

void copyin_boot_pdes(unsigned long *pd)
{
	memcpy(pd, boot_pd, PGSIZE);//BOOT_PTS_NR * sizeof(unsigned long));
}

void init_mmu(void)
{
	for (int i = 0; i < BOOT_PTS_NR; i++) {
		for (int j = 0; j < 1024; j++) {
			unsigned long page = (i * 1024 + j) * 4096;
			boot_pts[i][j] = page | PG_W | PG_P;
		}
		boot_pd[i] = (unsigned long)boot_pts[i] | PG_W | PG_P;
	}
	for (int i = BOOT_PTS_NR; i < 1024; i++)
		boot_pd[i] = 0;
#if 1
	static char __attribute__((aligned(4096))) zero_page[4096];
	boot_pts[0][0] = (unsigned long)zero_page | PG_P;
#else
	boot_pts[0][0] = 0;
#endif
	asm volatile ("mov %0, %%cr3" :: "r" (boot_pd));

	unsigned long cr0;
	asm volatile ("mov %%cr0, %0" : "=r" (cr0));
	asm volatile ("mov %0, %%cr0" :: "r" (cr0 | 0x80000000));
}
