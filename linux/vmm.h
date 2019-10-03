#pragma once

#include <sys/types.h>
#include <linux/segment.h>
#include <linux/mmu.h>

struct inode;

struct vm_region
{
	struct vm_region *prev, *next;
	unsigned long start, end;
	struct inode *ip;
	unsigned long size;
	off_t offset;
};

struct vm_page
{
	struct vm_page *prev, *next;
	unsigned long addr, pa, attr;
};

#define NR_VM_REGS	10
struct mm
{
	struct vm_region *regs;
	struct vm_page *pages;
	unsigned long *pgd;
};

int mmapi(struct mm *mm, struct inode *ip, off_t pos, void __user *p,
		size_t filesz, size_t memsz);
int do_vm_fault(struct mm *mm, unsigned long addr, unsigned int errcd);
struct mm *new_mm(void);
struct mm *fork_mm(struct mm *old_mm);
void free_mm(struct mm *mm);
void use_mm(struct mm *mm);
