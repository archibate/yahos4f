#pragma once

#define PG_P  	0x1     // Present
#define PG_W  	0x2     // Read / Write
#define PG_U  	0x4     // User / Kernel
#define PG_PWT	0x8     // Write through
#define PG_PCD	0x10    // Cache disable
#define PG_A  	0x20    // Accessed
#define PG_D  	0x40    // Dirty
#define PG_AVL	0xe00   // Avaliable for software use

#define PG_PAT	0x80    // PTE: Page Attribute Table (unused)
#define PG_PSE	0x80    // PDE: Page Size Extension (unused)

#define PGOFFS	0xfff
#define PGMASK	0xfffff000
#define PGSIZE	0x1000

void init_mmu(void);
