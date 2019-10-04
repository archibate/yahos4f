#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/segment.h>
#include <string.h>
#include <fcntl.h>

int sys_open(const char __user *path, int mode);
int sys_write(int fd, const void __user *buf, size_t size);
int sys_read(int fd, void __user *buf, size_t size);
off_t sys_lseek(int fd, off_t offset, int whence);
int sys_dup2(int fd, int dirfd);
int sys_dup(int fd);
int sys_close(int fd);

#define FDCHK(fd) \
	if (!(0 <= (fd) && (fd) < NR_OPEN && current->file[(fd)].f_inode)) \
		return -1; /* EBADF */
#define FDCHK2(fd) \
	if (!(0 <= (fd) && (fd) < NR_OPEN)) \
		return -1; /* EBADF */

static int alloc_empty_slot(void)
{
	for (int i = 0; i < NR_OPEN; i++) {
		if (!current->file[i].f_inode)
			return i;
	}
	return -1;
}

int sys_open(const char __user *path, int mode)
{
	int i = alloc_empty_slot();
	if (i == -1) return -1; /* EMFILE */
	struct file *f = &current->file[i];

	struct inode *ip = namei(path);
	if (!ip) return -1;
	if (S_ISDIR(ip->i_mode)) return -1; /* EISDIR */

	f->f_mode = mode;
	f->f_inode = ip;
	f->f_pos = 0;

	return i;
}

int sys_write(int fd, const void __user *buf, size_t size)
{
	FDCHK(fd);
	struct file *f = &current->file[fd];
	off_t offset;
	switch (f->f_mode & O_ACCMODE) {
	case O_WRONLY: case O_RDWR:
		offset = iwrite(f->f_inode, f->f_pos, buf, size);
		f->f_pos += offset;
		return offset;
	default:
		return -1; /* EPERM */
	}
}

int sys_read(int fd, void __user *buf, size_t size)
{
	FDCHK(fd);
	struct file *f = &current->file[fd];
	off_t offset;
	switch (f->f_mode & O_ACCMODE) {
	case O_RDONLY: case O_RDWR:
		offset = iread(f->f_inode, f->f_pos, buf, size);
		f->f_pos += offset;
		return offset;
	default:
		return -1; /* EPERM */
	}
}

off_t sys_lseek(int fd, off_t offset, int whence)
{
	FDCHK(fd);
	struct file *f = &current->file[fd];
	switch (whence) {
	case 0:
		f->f_pos = offset;
		break;
	case 1:
		f->f_pos += offset;
		break;
	case 2:
		f->f_pos = f->f_inode->i_size - offset;
		break;
	}
	return f->f_pos;
}

int sys_dup2(int fd, int dirfd)
{
	FDCHK(fd);
	FDCHK2(dirfd);
	if (fd == dirfd)
		return 0;
	sys_close(dirfd);
	struct file *g = &current->file[dirfd];
	struct file *f = &current->file[fd];
	memcpy(g, f, sizeof(struct file));
	f->f_inode = idup(g->f_inode);
	return 0;
}

int sys_dup(int fd)
{
	FDCHK(fd);
	int i = alloc_empty_slot();
	if (i == -1) return -1; /* EMFILE */
	struct file *g = &current->file[i];
	struct file *f = &current->file[fd];
	memcpy(g, f, sizeof(struct file));
	f->f_inode = idup(g->f_inode);
	return i;
}

int sys_close(int fd)
{
	FDCHK(fd);
	struct file *f = &current->file[fd];
	iput(f->f_inode);
	memset(f, 0, sizeof(struct file));
	return 0;
}
