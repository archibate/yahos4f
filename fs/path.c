#include <linux/kernel.h>
#include <linux/fs.h>
#include <string.h>
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

		if (0 > dir_find(dip, &de, name, 0))
			goto bad;
		int dev = dip->i_dev;
		iput(dip);
		dip = iget(dev, de.d_ino);
		if (!dip) {
			warning("iget returned NULL");
			goto bad;
		}
	}

	if (!S_ISDIR(dip->i_mode))
		goto bad;

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

	if (0 > dir_find(pip, &de, path, 0))
		goto bad;

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

struct inode *dir_creati(struct inode *dip, const char *path, unsigned int mode)
{
	struct inode *pip = dir_getp(dip, &path);
	if (!pip)
		return NULL;
	return dir_creat(pip, path, mode);
}

int dir_mkdiri(struct inode *dip, const char *path, unsigned int mode)
{
	struct inode *pip = dir_getp(dip, &path), *ip;
	if (!pip)
		return -1;
	ip = dir_creat(pip, path, (mode & ~S_IFMT) | S_IFDIR);
	if (!ip)
		return -1;
	if (dir_init(ip, pip) == -1) {
		warning("failed to dir_init");
		return -1;
	}
	return 0;
}

int dir_unlinki(struct inode *dip, const char *path)
{
	struct inode *pip = dir_getp(dip, &path);
	if (!pip)
		return -1;
	return dir_unlink(pip, path);
}
