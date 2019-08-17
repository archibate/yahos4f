#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/segment.h>
#include <elf.h>

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

static size_t iread_user(struct inode *ip, off_t pos, void *p, size_t size)
{
	size_t n = size;
	static char buf[4096];
	while (size > 0) {
		if (n > 4096)
			n = 4096;
		iread(ip, pos, buf, n);
		copy_to_user(p, buf, n);
		size -= n;
		pos += n;
		p += n;
	}
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
		iread_user(ip, ph.p_offset, (void *)ph.p_paddr, ph.p_filesz);
		clear_user((void *)ph.p_paddr + ph.p_filesz,
				ph.p_memsz - ph.p_filesz);
		if (ebss < ph.p_paddr + ph.p_memsz)
			ebss = ph.p_paddr + ph.p_memsz;
	}

	current->user_entry = e.e_entry;
	current->ebss = ebss;
	current->brk = ebss;
	current->stop = ebss + 4096;

	current->euid = euid;
	current->egid = egid;
	return 0;
}

int do_execve(const char *path)
{
	struct inode *ip = namei(path);
	if (!ip) return -1;
	int ret = execve_inode(ip);
	iput(ip);
	return ret;
}
