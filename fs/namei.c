#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <string.h>

static struct inode *path_name(const char **pp)
{
	struct inode *dip;
	if (**pp == '/') {
		dip = iget(ROOT_DEV, ROOT_INO);
		if (!dip) {
			warning("iget(ROOT) == NULL");
			return NULL;
		}
		for (; **pp == '/'; ++*pp);
	} else {
		dip = current->cwd;
		if (!dip) {
			warning("current->cwd == NULL");
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
	pip = dir_getp(dip, ppath);
	iput(dip);
	return pip;
}

struct inode *namei(const char *path)
{
	struct inode *dip, *ip;
	if (!(dip = path_name(&path)))
		return NULL;
	ip = dir_geti(dip, path);
	iput(dip);
	return ip;
}

int linki(const char *path, struct inode *ip)
{
	int ret;
	struct inode *dip;
	if (!(dip = path_name(&path)))
		return 0;
	ret = dir_linki(dip, path, ip);
	iput(dip);
	return ret;
}
