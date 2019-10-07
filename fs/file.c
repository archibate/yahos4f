#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/segment.h>
#include <dirent.h>
#include <string.h>
#include <alloca.h>
#include <fcntl.h>

int sys_open(const char __user *path, int oflags, int mode);
int sys_write(int fd, const void __user *buf, size_t size);
int sys_read(int fd, void __user *buf, size_t size);
off_t sys_lseek(int fd, off_t offset, int whence);
int sys_dirread(int fd, struct dirent __user *ent);
int sys_dirrewind(int fd);
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

int sys_open(const char __user *path, int oflags, int mode)
{
	int i = alloc_empty_slot();
	if (i == -1) return -1; /* EMFILE */
	struct file *f = &current->file[i];

	struct inode *ip = namei(path);
	if (ip && (oflags & O_EXCL))
		return -1; /* EEXIST */
	if (!ip) {
		if (!(oflags & O_CREAT))
			return -1;
		ip = creati(path, mode);
		if (!ip)
			return -1;
	}
	if (S_ISDIR(ip->i_mode) && !(oflags & O_DIRECTORY)) return -1; /* EISDIR */
	if (!S_ISDIR(ip->i_mode) && (oflags & O_DIRECTORY)) return -1; /* ENOTDIR */
	if (oflags & O_TRUNC) {
		ip->i_size = 0;
		iupdate(ip);
	}

	f->f_oflags = oflags;
	f->f_inode = ip;
	f->f_pos = (oflags & O_APPEND) ? ip->i_size : 0;

	return i;
}

int sys_write(int fd, const void __user *buf, size_t size)
{
	FDCHK(fd);
	struct file *f = &current->file[fd];
	off_t offset;
	switch (f->f_oflags & (O_ACCMODE | O_DIRECTORY)) {
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
	switch (f->f_oflags & (O_ACCMODE | O_DIRECTORY)) {
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
	if (f->f_oflags & O_DIRECTORY)
		return -1; /* EPERM */
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

void do_inode_stat(struct inode *ip, struct stat *st)
{
	memset(st, 0, sizeof(struct stat));
	st->st_dev = ip->i_dev;
	st->st_ino = ip->i_ino;
	st->st_mode = ip->i_mode;
	st->st_nlink = ip->i_nlinks;
	st->st_uid = ip->i_uid;
	st->st_gid = ip->i_gid;
	st->st_rdev = (S_ISCHR(ip->i_mode) || S_ISBLK(ip->i_mode)) * ip->i_zone[0];
	st->st_size = ip->i_size;
	st->st_atime = ip->i_atime;
	st->st_mtime = ip->i_mtime;
	st->st_ctime = ip->i_ctime;
}

int sys_fstatat(int fd, const char __user *path, struct stat __user *st, int flags)
{
	struct inode *old_cwd = current->cwd;
	if (fd != AT_FDCWD) {
		FDCHK(fd);
		struct file *f = &current->file[fd];
		if (!(f->f_oflags & O_DIRECTORY))
			return -1; /* EPERM */
		current->cwd = f->f_inode;
	}
	struct inode *ip = namei(path);
	current->cwd = old_cwd;
	if (!ip) return -1;
	do_inode_stat(ip, st);
	iput(ip);
	return 0;
}

int sys_dirread(int fd, struct dirent __user *ent)
{
	FDCHK(fd);
	struct file *f = &current->file[fd];
	if (!(f->f_oflags & O_DIRECTORY))
		return -1; /* EPERM */
	struct dir_entry de;
	size_t offset = iread(f->f_inode, f->f_pos, &de, sizeof(de));
	if (offset != sizeof(de))
		return 0;
	if (de.d_name_len + 1 >= sizeof(ent->d_name))
		return -1; /* ENAMETOOLONG */
	char *name = alloca(de.d_name_len + 1);
	offset = iread(f->f_inode, f->f_pos + offset, name, de.d_name_len);
	if (offset != de.d_name_len)
		return 0;
	name[offset] = 0;
	ent->d_ino = de.d_ino;
	ent->d_off = f->f_pos;
	ent->d_type = de.d_type;
	ent->d_reclen = sizeof(struct dirent) - sizeof(ent->d_name) + de.d_name_len;
	strcpy(ent->d_name, name);
	f->f_pos += de.d_entry_size;
	return 1;
}

int sys_dirrewind(int fd)
{
	FDCHK(fd);
	struct file *f = &current->file[fd];
	if (f->f_oflags & O_DIRECTORY)
		return -1; /* EPERM */
	f->f_pos = 0;
	return 0;
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
