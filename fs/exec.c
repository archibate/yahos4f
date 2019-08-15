#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/segment.h>
#include <elf.h>

static int is_vaild_elf_header(struct Elf32_Ehdr *e)
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

static size_t iread_fs(struct inode *ip, off_t pos, void *p, size_t size)
{
	size_t n = size;
	static char buf[PAGE_SIZE];
	while (size > 0) {
		if (n > PAGE_SIZE)
			n = PAGE_SIZE;
		iread(ip, pos, buf, n);
		write_fs_memory(p, buf, n);
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

	static struct Elf32_Ehdr e;
	if (iread(ip, 0, &e, sizeof(e)) != sizeof(e))
		return -1; /* ENOEXEC */
	if (!is_vaild_elf_header(e))
		return -1; /* ENOEXEC */

	if (current->executable)
		iput(current->executable);
	current->executable = ip;
	proc_close_current();

	static struct Elf32_Phdr ph;
	for (int i = 0; i < e->e_phnum; i++) {
		iread(ip, e->e_phoff + i * sizeof(ph), &ph, sizeof(ph));
		iread_fs(ip, ph->p_offset, (void *)ph->p_paddr, ph->p_filesz);
		zero_fs_memory((void *)ph->p_paddr + ph->p_filesz,
				ph->p_memsz - ph->p_filesz);
	}

	current->ctx.pc = e->e_entry;

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
