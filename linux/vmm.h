#pragma once

#include <sys/types.h>
#include <linux/segment.h>
#include <linux/mmu.h>

struct inode;

enum vm_type
{
	VM_FAULT = 0,
	VM_BSS,
	VM_FILE,
};

struct vm_region
{
	struct vm_region *prev, *next;
	unsigned long start, end;
	enum vm_type type;
	struct inode *ip;
	off_t offset;
};

#define NR_VM_REGS	10
struct mm
{
	struct vm_region *regs;
	unsigned long *pgd;
};

int mmapz(struct mm *mm, void __user *p, size_t size);
int mmapi(struct mm *mm, struct inode *ip, off_t pos, void __user *p, size_t size);
int do_vm_fault(struct mm *mm, unsigned long addr);
struct mm *new_mm(void);
void free_mm(struct mm *mm);
void use_mm(struct mm *mm);
