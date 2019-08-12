#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <string.h>

int namep(struct dir_entry *de, const char *path, struct inode **ppip)
{
	int pos;
	struct inode *dip;
	if (path[0] == '/') {
		dip = iget(ROOT_DEV, ROOT_INO);
		if (!dip) {
			warning("iget(ROOT) == NULL");
			return -1;
		}
		for (; *path == '/'; path++);
	} else {
		dip = current->cwd;
		if (!dip) {
			warning("current->cwd == NULL");
			return -1;
		}
	}
	pos = dir_getp(dip, de, path, ppip);
	iput(dip);
	return pos;
}

struct inode *namei(const char *path)
{
	struct inode *dip, *ip;
	if (path[0] == '/') {
		dip = iget(ROOT_DEV, ROOT_INO);
		if (!dip) {
			warning("iget(ROOT) == NULL");
			return NULL;
		}
		for (; *path == '/'; path++);
	} else {
		dip = current->cwd;
		if (!dip) {
			warning("current->cwd == NULL");
			return NULL;
		}
	}
	ip = dir_geti(dip, path);
	iput(dip);
	return ip;
}
