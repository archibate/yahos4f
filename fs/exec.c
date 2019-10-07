#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/vmm.h>
#include <linux/user.h>
#include <linux/eflags.h>
#include <linux/mman.h>
#include <string.h>
#include <elf.h>

#define USER_STACK_SIZE (8192*1024)

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

static int stack_push_arguments(void **psp, char *const *argv)
{
	int argc;
	void *sp = *psp;
	for (argc = 0; argv[argc]; argc++);
	char *argv_usr[argc];
	for (int i = argc - 1; i >= 0; i--) {
		sp -= strlen(argv[i]) + 1;
		strcpy(sp, argv[i]);
		argv_usr[i] = sp;
	}
	sp = (void *)((unsigned long)sp & -8);
	sp -= sizeof(char *);
	*(char **)sp = NULL;
	sp -= sizeof(char *) * argc;
	memcpy(sp, argv_usr, sizeof(char *) * argc);
	*psp = sp;
	return argc;
}

static int execve_inode(struct inode *ip, char *const *argv, char *const *envp)
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
	if (!(i & S_IXOTH))
		return -1; /* EACCES */

	static Elf32_Ehdr e;
	if (iread(ip, 0, &e, sizeof(e)) != sizeof(e))
		return -1; /* ENOEXEC */
	if (!is_vaild_elf_header(&e))
		return -1; /* ENOEXEC */

	destroy_user_task(current);
	current->mm = new_mm();
	use_mm(current->mm);
	current->executable = ip;

	unsigned long ebss = 0;
	static Elf32_Phdr ph;
	for (int i = 0; i < e.e_phnum; i++) {
		iread(ip, e.e_phoff + i * sizeof(ph), &ph, sizeof(ph));
		mmapi(current->mm, ip, ph.p_offset, (void __user *)ph.p_paddr,
				ph.p_filesz, ph.p_memsz);
		if (ebss < ph.p_paddr + ph.p_memsz)
			ebss = ph.p_paddr + ph.p_memsz;
	}

	ebss = (ebss + PGSIZE - 1) & PGMASK;
	current->ebss = ebss;
	current->brk = ebss;
	current->stop = ebss + USER_STACK_SIZE - 8;
	mmapi(current->mm, NULL, 0, (void __user *)ebss, 0, USER_STACK_SIZE);

	void __user *sp = (void __user *)current->stop;
	stack_push_arguments(&sp, envp);
	char __user * __user *envp_usr = sp;
	int argc = stack_push_arguments(&sp, argv);
	char __user * __user *argv_usr = sp;
	sp -= sizeof(void *);
	*(char __user * __user * __user *)sp = envp_usr;
	sp -= sizeof(void *);
	*(char __user * __user * __user *)sp = argv_usr;
	sp -= sizeof(int);
	*(int __user *)sp = argc;
	sp -= sizeof(void *);
	*(void * __user *)sp = NULL;
	current->stop = (unsigned long)sp;

	current->euid = euid;
	current->egid = egid;

	move_to_user(e.e_entry, current->stop, FL_1F | FL_IF);
}

int do_execve(const char *path, char *const *argv, char *const *envp)
{
	struct inode *ip = namei(path);
	if (!ip) return -1;
	int ret = execve_inode(ip, argv, envp);
	iput(ip);
	return ret;
}
