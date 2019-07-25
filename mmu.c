#include "mmu.h"

#define BOOT_PTS_NR (0x400000 / (4096*1024))

static unsigned long __attribute__((aligned(4096))) boot_pts[BOOT_PTS_NR][1024];
static unsigned long __attribute__((aligned(4096))) boot_pd[1024];

void setup_pagging(void)
{
	for (int i = 0; i < BOOT_PTS_NR; i++) {
		for (int j = 0; j < 1024; j++) {
			unsigned long page = (i * 1024 + j) * 4096;
			boot_pts[i][j] = page | PG_W | PG_P;
		}
		boot_pd[i] = (unsigned long)boot_pts[i] | PG_W | PG_P;
	}
	asm volatile ("mov %0, %%cr3" :: "r" (boot_pd));

	unsigned long cr0;
	asm volatile ("mov %%cr0, %0" : "=r" (cr0));
	asm volatile ("mov %0, %%cr0" :: "r" (cr0 | 0x80000000));
}
