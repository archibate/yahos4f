#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <string.h>
#include <errno.h>

static struct inode *path_name(const char **pp)
{
	struct inode *dip;
	if (**pp == '/') {
		dip = iget(ROOT_DEV, ROOT_INO);
		if (!dip)
			panic("iget(ROOT) == NULL");
		for (; **pp == '/'; ++*pp);
	} else {
		dip = idup(current->cwd);
		if (!dip) {
			warning("current->cwd == NULL");
			errno = ENOENT;
			return NULL;
		}
	}
	return dip;
}

struct inode *namep(const char **ppath)
{
	int pos;
	struct inode *dip, *pip;
	if (!(dip = path_name(ppath)))
		return NULL;
	if (!**ppath)
		return dip;
	pip = dir_getp(dip, ppath);
	iput(dip);
	return pip;
}

struct inode *namei(const char *path)
{
	struct inode *dip, *ip;
	if (!(dip = path_name(&path)))
		return NULL;
	if (!*path)
		return dip;
	ip = dir_geti(dip, path);
	iput(dip);
	return ip;
}

int fs_unlink(const char *path)
{
	int ret;
	struct inode *dip;
	if (!(dip = path_name(&path)))
		return -1;
	ret = dir_unlinki(dip, path);
	iput(dip);
	return ret;
}

int linki(const char *path, struct inode *ip)
{
	int ret;
	struct inode *dip;
	if (!(dip = path_name(&path)))
		return -1;
	ret = dir_linki(dip, path, ip);
	iput(dip);
	return ret;
}

struct inode *creati(const char *path, unsigned int mode)
{
	struct inode *dip, *ip;
	if (S_ISCHR(mode) || S_ISBLK(mode))
		return NULL;
	if (!(dip = path_name(&path)))
		return NULL;
	ip = dir_creati(dip, path, mode, 0);
	iput(dip);
	return ip;
}

int fs_mknod(const char *path, unsigned int mode, unsigned int nod)
{
	struct inode *dip, *ip;
	if (!S_ISCHR(mode) && !S_ISBLK(mode))
		return -1;
	if (!(dip = path_name(&path)))
		return -1;
	ip = dir_creati(dip, path, mode, nod);
	iput(dip);
	if (ip) iput(ip);
	return ip ? 0 : -1;
}

int fs_mkdir(const char *path, unsigned int mode)
{
	int ret;
	struct inode *dip;
	if (!(dip = path_name(&path)))
		return -1;
	ret = dir_mkdiri(dip, path, mode);
	iput(dip);
	return ret;
}

int fs_rmdir(const char *path)
{
	int ret;
	struct inode *dip;
	if (!(dip = path_name(&path)))
		return -1;
	ret = dir_rmdiri(dip, path);
	iput(dip);
	return ret;
}

int fs_link(const char *oldpath, const char *newpath)
{
	struct inode *ip = namei(oldpath);
	if (!ip) return -1;
	int ret = linki(newpath, ip);
	iput(ip);
	return ret;
}
