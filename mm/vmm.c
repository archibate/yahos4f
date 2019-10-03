#include <linux/kernel.h>
#include <linux/segment.h>
#include <linux/mman.h>
#include <linux/vmm.h>
#include <linux/pmm.h>
#include <linux/mmu.h>
#include <linux/fs.h>
#include <string.h>

static int map_page(struct mm *mm, void __user *p,
		void *page, unsigned int attr);


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


static struct vm_region *duplicate_vm_region(struct vm_region *ovm)
{
	struct vm_region *vm = calloc(1, sizeof(struct vm_region));
	vm->start = ovm->start;
	vm->end = ovm->end;
	vm->size = ovm->size;
	vm->offset = ovm->offset;
	vm->ip = idup(ovm->ip);
	return vm;
}

struct mm *fork_mm(struct mm *old_mm)
{
	struct mm *mm = new_mm();
	struct vm_region *ovm, *vm, **pprev = &mm->regs;
	for (ovm = old_mm->regs; ovm; ovm = ovm->next) {
		vm = duplicate_vm_region(ovm);
		vm->prev = *pprev;
		*pprev = vm;
		vm->next = vm;
		pprev = &vm->next;
	}
	*pprev = NULL;
	struct vm_page *opg;
	for (opg = old_mm->pages; opg; opg = opg->next) {
		void *page = alloc_page();
		memcpy(page, (void *)opg->pa, PGSIZE);
		map_page(mm, (void __user *)opg->addr,
				page, opg->attr);
	}
	return mm;
}


static struct vm_region *new_vm_region(struct mm *mm, void __user *p,
		size_t filesz, size_t memsz)
{
	unsigned long start = (unsigned long)p;
	if (start & PGOFFS) {
		warning("start addr %p not aligned", start);
		return NULL; /* EINVAL */
	}
	unsigned end = (start + memsz + PGSIZE - 1) & PGMASK;

	struct vm_region *vm;
	for (vm = mm->regs; vm; vm = vm->next) {
		if (	(vm->start <= start && start < vm->end) ||
			(vm->start <= end && end < vm->end)) {
			warning("vm_region overlapped %p:%p",
					vm->start, vm->end);
			return NULL; /* EFAULT */
		}
	}
	vm = calloc(1, sizeof(struct vm_region));
	vm->start = start;
	vm->end = end;
	vm->size = filesz;
	vm->next = mm->regs;
	if (mm->regs) mm->regs->prev = vm;
	vm->prev = NULL;
	mm->regs = vm;
	return vm;
}

int mmapi(struct mm *mm, struct inode *ip, off_t pos, void __user *p,
		size_t filesz, size_t memsz)
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


static struct vm_page *get_vm_page(struct mm *mm, unsigned long addr)
{
	for (struct vm_page *pg = mm->pages; pg; pg = pg->next)
		if (pg->addr == addr)
			return pg;
	return NULL;
}

static void mark_mapped_page(struct mm *mm, unsigned long addr,
		unsigned long pa, unsigned int attr)
{
	struct vm_page *pg = get_vm_page(mm, addr);
	if (!pg) { // new_vm_pageA?
		pg = calloc(1, sizeof(struct vm_page));
		pg->addr = addr;
		pg->next = mm->pages;
		if (mm->pages) mm->pages->prev = pg;
		pg->prev = NULL;
		mm->pages = pg;
	}
	pg->attr = attr;
	pg->pa = pa;
}

static int do_vm_region_fault(struct mm *mm,
		struct vm_region *vm, unsigned long addr, unsigned int errcd)
{
	addr &= PGMASK;
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
	ret = map_page(mm, (void __user *)addr, page, errcd);
	if (ret < 0)
		free_page(page);
	return ret;
}

int do_vm_fault(struct mm *mm, unsigned long addr, unsigned int errcd)
{
	addr &= PGMASK;
	struct vm_region *vm;
	for (vm = mm->regs; vm; vm = vm->next) {
		//printk("!! %p %p %p %d", vm->start, addr, vm->end, vm->type);
		if (vm->start <= addr && addr < vm->end)
			return do_vm_region_fault(mm, vm, addr, errcd);
	}
	return -1;
}

static int pgd_map_page(unsigned long *pgd, unsigned long va,
		unsigned long pa, unsigned int attr)
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
	/*if (*pte & PG_P) {
		warning("%p already mapped (pte=%p, npa=%p)", va, *pte, pa);
	}*/
	*pte = (pa & PGMASK) | PG_U | (attr & PG_W) | PG_P;
	return 0;
}

#if 0
static unsigned long pgd_get_mapping(unsigned long *pgd, unsigned long va)
{
	unsigned long pde = pgd[va >> 22], *pt, pte;
	if (!(pde & PG_P))
		return 0;
	pt = (void *)(pde & PGMASK);
	pte = pt[(va >> 12) & 0x3ff];
	if (pte & PG_P)
		return pte;
	return 0;
}
#endif

static int map_page(struct mm *mm, void __user *p,
		void *page, unsigned int attr)
{
	//printk("map_page %p %p (*=%p)", p, page, 0[(int*)page]);
	if (!mm->pgd) panic("mm->pgd == NULL");
	int ret = pgd_map_page(mm->pgd, (unsigned long)p,
			(unsigned long)page, attr);
	if (ret >= 0) {
		asm volatile ("invlpg (%0)" :: "r" (p) : "cc", "memory");
		mark_mapped_page(mm, (unsigned long)p,
				(unsigned long)page, attr);
	}
	return ret;
}

#if 0
static unsigned long get_mapping(struct mm *mm, void __user *p)
{
	if (!mm->pgd) panic("mm->pgd == NULL");
	unsigned long pte = pgd_get_mapping(mm->pgd, (unsigned long)p);
	return pte;
}
#endif
