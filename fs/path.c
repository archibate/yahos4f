#include <linux/kernel.h>
#include <linux/fs.h>
#include <string.h>
#include <errno.h>
#include <alloca.h>

struct inode *dir_getp(struct inode *dip, const char **ppath)
{
	static struct dir_entry de;
	char *name;
	const char *p, *path = *ppath;
	int pos;

	dip = idup(dip);

	while ((p = strchr(path, '/'))) {
		name = alloca(p - path + 1);
		memcpy(name, path, p - path);
		name[p - path] = 0;

		for (; *p == '/'; p++);
		path = p;

		if (0 > dir_find(dip, &de, name, 0)) {
			errno = ENOENT;
			goto bad;
		}
		int dev = dip->i_dev;
		iput(dip);
		dip = iget(dev, de.d_ino);
		if (!dip) {
			warning("iget returned NULL");
			goto bad;
		}
	}

	if (!S_ISDIR(dip->i_mode)) {
		errno = ENOTDIR;
		goto bad;
	}

	*ppath = path;
	return dip;

bad:
	iput(dip);
	return NULL;
}

struct inode *dir_geti(struct inode *dip, const char *path)
{
	static struct dir_entry de;
	struct inode *ip, *pip = dir_getp(dip, &path);
	if (!pip) return NULL;

	if (0 > dir_find(pip, &de, path, 0)) {
		errno = ENOENT;
		goto bad;
	}

	int dev = pip->i_dev;
	iput(pip);
	ip = iget(dev, de.d_ino);
	if (!ip)
		warning("iget(pip->i_dev, de.d_ino) returned NULL");
	return ip;
bad:
	iput(pip);
	return NULL;
}

int dir_linki(struct inode *dip, const char *path, struct inode *ip)
{
	static struct dir_entry de;
	struct inode *pip = dir_getp(dip, &path);
	if (!pip) return -1;

	if (0 > dir_link(pip, path, ip))
		goto bad;
	iput(pip);
	return 0;
bad:
	iput(pip);
	return -1;
}

struct inode *dir_creati(struct inode *dip, const char *path, unsigned int mode,
		unsigned int nod)
{
	struct inode *pip = dir_getp(dip, &path), *ip;
	if (!pip)
		return NULL;
	ip = dir_creat(pip, path, mode, nod);
	iput(pip);
	return ip;
}

int dir_mkdiri(struct inode *dip, const char *path, unsigned int mode)
{
	struct inode *pip = dir_getp(dip, &path), *ip;
	if (!pip)
		return -1;
	ip = dir_creat(pip, path, (mode & ~S_IFMT) | S_IFDIR, 0);
	if (!ip)
		goto bad;
	if (dir_init(ip, pip) == -1) {
		warning("failed to dir_init");
		dir_unlink(pip, path, 2);
		goto bad;
	}
	iput(pip);
	iput(ip);
	return 0;
bad:
	iput(pip);
	return -1;
}

int dir_unlinki(struct inode *dip, const char *path)
{
	struct inode *pip = dir_getp(dip, &path);
	if (!pip)
		return -1;
	int ret = dir_unlink(pip, path, 0);
	iput(pip);
	return ret;
}

int dir_rmdiri(struct inode *dip, const char *path)
{
	struct inode *pip = dir_getp(dip, &path);
	if (!pip)
		return -1;
	int ret = dir_unlink(pip, path, 1);
	iput(pip);
	return ret;
}
