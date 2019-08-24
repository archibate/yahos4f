#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/vmm.h>
#include <linux/user.h>
#include <linux/eflags.h>
#include <elf.h>

#define USER_STACK_SIZE 8192

static int is_vaild_elf_header(Elf32_Ehdr *e)
{
	return	1
		&& e->e_ident[EI_MAG0] == ELFMAG0
		&& e->e_ident[EI_MAG1] == ELFMAG1
		&& e->e_ident[EI_MAG2] == ELFMAG2
		&& e->e_ident[EI_MAG3] == ELFMAG3
		&& e->e_machine == EM_386
		&& e->e_type == ET_EXEC
		;
}

static int proc_close_current(void)
{
	// ...
}

static int execve_inode(struct inode *ip)
{
	if (S_ISDIR(ip->i_mode))
		return -1; /* EISDIR */
	if (!S_ISREG(ip->i_mode))
		return -1; /* EACCES */

	unsigned int i = ip->i_mode;
	int euid = (i & S_ISUID) ? ip->i_uid : current->euid;
	int egid = (i & S_ISGID) ? ip->i_gid : current->egid;
	if (current->euid == ip->i_uid)
		i >>= 6;
	else if (current->egid == ip->i_gid)
		i >>= 3;
	if (!(i & S_XOTH))
		return -1; /* EACCES */

	static Elf32_Ehdr e;
	if (iread(ip, 0, &e, sizeof(e)) != sizeof(e))
		return -1; /* ENOEXEC */
	if (!is_vaild_elf_header(&e))
		return -1; /* ENOEXEC */

	if (current->executable)
		iput(current->executable);
	current->executable = ip;
	proc_close_current();

	unsigned long ebss = 0;
	static Elf32_Phdr ph;
	for (int i = 0; i < e.e_phnum; i++) {
		iread(ip, e.e_phoff + i * sizeof(ph), &ph, sizeof(ph));
		mmapi(current->mm, ip, ph.p_offset,
				(void __user *)ph.p_paddr, ph.p_filesz);
		mmapz(current->mm, (void __user *)ph.p_paddr + ph.p_filesz,
				ph.p_memsz - ph.p_filesz);
		if (ebss < ph.p_paddr + ph.p_memsz)
			ebss = ph.p_paddr + ph.p_memsz;
	}

	ebss = (ebss + PGSIZE - 1) & PGMASK;
	current->ebss = ebss;
	current->brk = ebss;
	current->stop = ebss + USER_STACK_SIZE;
	mmapz(current->mm, (void __user *)ebss, USER_STACK_SIZE);

	current->euid = euid;
	current->egid = egid;

	move_to_user(e.e_entry, current->stop - 8, FL_1F | FL_IF);
}

int do_execve(const char *path)
{
	struct inode *ip = namei(path);
	if (!ip) return -1;
	int ret = execve_inode(ip);
	iput(ip);
	return ret;
}
