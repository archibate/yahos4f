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
			goto not_found;
		int dev = dip->i_dev;
		iput(dip);
		dip = iget(dev, de.d_ino);
		if (!dip) {
			warning("iget returned NULL");
			goto not_found;
		}
	}

	if (!S_ISDIR(dip->i_mode))
		goto not_found;

	*ppath = path;
	return dip;

not_found:
	iput(dip);
	return NULL;
}

struct inode *dir_geti(struct inode *dip, const char *path)
{
	static struct dir_entry de;
	struct inode *ip, *pip = dir_getp(dip, &path);
	if (!pip) return NULL;

	if (0 > dir_find(pip, &de, path, 0))
		goto not_found;

	int dev = pip->i_dev;
	iput(pip);
	ip = iget(dev, de.d_ino);
	if (!ip)
		warning("iget(pip->i_dev, de.d_ino) returned NULL");
	return ip;
not_found:
	iput(pip);
	return NULL;
}

int dir_linki(struct inode *dip, const char *path, struct inode *ip)
{
	static struct dir_entry de;
	struct inode *pip = dir_getp(dip, &path);
	if (!pip) return 0;

	if (0 > dir_link(pip, path, ip))
		goto already_exist;
	iput(pip);
	return 1;
already_exist:
	iput(pip);
	return 0;
}
