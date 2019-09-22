#include <linux/kernel.h>
#include <linux/segment.h>
#include <linux/mman.h>
#include <linux/vmm.h>
#include <linux/pmm.h>
#include <linux/mmu.h>
#include <linux/fs.h>
#include <string.h>

struct mm *new_mm(void)
{
	struct mm *mm = calloc(1, sizeof(struct mm));
	mm->pgd = alloc_page();
	copyin_boot_pdes(mm->pgd);
	return mm;
}

void use_mm(struct mm *mm)
{
	asm volatile ("mov %0, %%cr3" :: "r" (mm->pgd));
}

void free_mm(struct mm *mm)
{
	free_page(mm->pgd);
	free(mm);
}

static int map_page(struct mm *mm, void __user *p, void *page);

struct vm_region *new_vm_region(struct mm *mm, void __user *p, size_t filesz, size_t memsz)
{
	unsigned long start = (unsigned long)p;
	if (start & PGOFFS) {
		warning("start addr %p not aligned", p);
		return NULL; /* EINVAL */
	}
	unsigned end = (start + memsz + PGSIZE - 1) & PGMASK;

	struct vm_region *vm;
	for (vm = mm->regs; vm; vm = vm->next) {
		if (	(vm->start <= start && start < vm->end) ||
			(vm->start <= end && end < vm->end)) {
			warning("map vm range overlapped at %p", p);
			return NULL; /* EFAULT */
		}
	}
	vm = calloc(1, sizeof(struct vm_region));
	vm->start = start;
	vm->end = end;
	vm->size = filesz;
	vm->next = mm->regs;
	vm->prev = NULL;
	mm->regs = vm;
	return vm;
}

int mmapi(struct mm *mm, struct inode *ip, off_t pos, void __user *p, size_t filesz, size_t memsz)
{
	if (filesz > memsz)
		memsz = filesz;
	if (!memsz)
		return 0;
	if (!ip && filesz)
		return -1; /* EINVAL */
	//printk("mmap %p %p D%dI%d@%#x", p, size, ip->i_dev,ip->i_ino, pos);
	struct vm_region *vm = new_vm_region(mm, p, filesz, memsz);
	if (!vm) return -1;
	vm->ip = ip;
	vm->offset = pos;
	return 0;
}

static int do_vm_region_fault(struct mm *mm,
		struct vm_region *vm, unsigned long addr)
{
	int ret = 0;
	void *page = alloc_page();
	size_t off = addr - vm->start;
	size_t size_read = 0;
	if (off < vm->size) {
		size_t size = vm->size - off;
		if (size > PGSIZE)
			size = PGSIZE;
		size_read = iread(vm->ip, vm->offset + off, page, size);
		if (size_read != size)
			warning("iread(vm->ip) != size");
	}
	memset(page + size_read, 0, PGSIZE - size_read);
	ret = map_page(mm, (void __user *)addr, page);
out:
	if (ret < 0)
		free_page(page);
	return ret;
}

int do_vm_fault(struct mm *mm, unsigned long addr)
{
	addr &= PGMASK;
	struct vm_region *vm;
	for (vm = mm->regs; vm; vm = vm->next) {
		//printk("!! %p %p %p %d", vm->start, addr, vm->end, vm->type);
		if (vm->start <= addr && addr < vm->end)
			return do_vm_region_fault(mm, vm, addr);
	}
	return -1;
}

static int pgd_map_page(unsigned long *pgd, unsigned long va, unsigned long pa)
{
	unsigned long *pde = &pgd[va >> 22], *pt, *pte;
	if (!(*pde & PG_P)) {
		pt = alloc_page();
		memset(pt, 0, PGSIZE);
		*pde = (unsigned long)pt | PG_U | PG_W | PG_P;
	} else {
		if (!(*pde & PG_U))
			panic("trying to map page in kernel section");
		pt = (void *)(*pde & PGMASK);
	}
	pte = &pt[(va >> 12) & 0x3ff];
	if (*pte & PG_P) {
		warning("%p already mapped (pte=%p, npa=%p)", va, *pte, pa);
		return -2;
	}
	*pte = (pa & PGMASK) | PG_U | PG_W | PG_P;
	return 0;
}

static int map_page(struct mm *mm, void __user *p, void *page)
{
	//printk("map_page %p %p (*=%p)", p, page, 0[(int*)page]);
	if (!mm->pgd) panic("mm->pgd == NULL");
	int ret = pgd_map_page(mm->pgd, (unsigned long)p, (unsigned long)page);
	asm volatile ("invlpg (%0)" :: "r" (p) : "cc", "memory");
	return ret;
}
