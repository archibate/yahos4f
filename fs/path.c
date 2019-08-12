#include <linux/kernel.h>
#include <linux/fs.h>
#include <string.h>
#include <alloca.h>

int dir_getp(struct inode *dip, struct dir_entry *de, const char *path,
		struct inode **ppip)
{
	char *name;
	const char *p;
	int pos;

	dip = idup(dip);

	while ((p = strchr(path, '/'))) {
		name = alloca(p - path + 1);
		memcpy(name, path, p - path);
		name[p - path] = 0;

		for (; *p == '/'; p++);
		path = p;

		if (-1 == dir_find(dip, de, name, 0))
			goto not_found;
		dip = iget(dip->i_dev, de->d_ino);
	}

	if (-1 == (pos = dir_find(dip, de, path, 0)))
		goto not_found;

	*ppip = dip;
	return pos;

not_found:
	iput(dip);
	return -1;
}

struct inode *dir_geti(struct inode *dip, const char *path)
{
	static struct dir_entry de;
	struct inode *pip, *ip;
	int pos = dir_getp(dip, &de, path, &pip);
	if (pos == -1)
		return NULL;

	int dev = pip->i_dev;
	iput(pip);
	ip = iget(dev, de.d_ino);
	if (!ip)
		warning("iget(pip->i_dev, de.d_ino) returned NULL");
	return ip;
}
