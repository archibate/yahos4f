#include <linux/kernel.h>
#include <linux/fs.h>
#include <string.h>
#include <alloca.h>

int dir_find(struct inode *dip, struct dir_entry *de, const char *na, off_t pos)
{
	char *name = alloca(20);
	size_t last_name_len = 20;
	int ret;

	if (!S_ISDIR(dip->i_mode))
		return -1;

	while (iread(dip, pos, de, DIR_ENTRY_SIZE) == DIR_ENTRY_SIZE) {
		if (de->d_name_len >= last_name_len) {
			name = alloca(de->d_name_len + 1);
			last_name_len = de->d_name_len;
		}
		de->d_name_len = iread(dip, pos + DIR_ENTRY_SIZE,
				name, de->d_name_len);
		name[de->d_name_len] = 0;
		pos += de->d_entry_size;
		if (!strcmp(na, name))
			return pos;
	}
	return -1 - pos;
}

int dir_unlink(struct inode *dip, const char *name)
{
	if (!S_ISDIR(dip->i_mode))
		return -1;

	static struct dir_entry de;
	int pos = dir_find(dip, &de, name, 0);
	if (pos < 0)
		return -1;

	int ino = de.d_ino;
	size_t len = de.d_name_len;
	de.d_ino = 0;
	de.d_name_len = 0;
	de.d_type = 0;

	if (iwrite(dip, pos, &de, DIR_ENTRY_SIZE) != DIR_ENTRY_SIZE) {
		warning("cannot iwrite dir del entry");
		return -1;
	}
	char *zero = alloca(len);
	if (iwrite(dip, pos + DIR_ENTRY_SIZE, zero, len) != len) {
		warning("cannot iwrite zero entry name string");
		return -1;
	}
	struct inode *ip = iget(dip->i_dev, ino);
	if (!ip) {
		warning("iget returned NULL");
	}
	ip->i_nlinks--;
	iupdate(ip);
	iput(ip);
	return ino;
}

int dir_link(struct inode *dip, const char *name, struct inode *ip)
{
	if (!S_ISDIR(dip->i_mode))
		return -1;

	static struct dir_entry de;
	int pos = dir_find(dip, &de, name, 0);
	if (pos >= 0)
		return -1 - pos;
	pos = -1 - pos;

	size_t len = strlen(name);
	de.d_ino = ip->i_ino;
	de.d_name_len = len;
	de.d_entry_size = sizeof(de) + len;
	de.d_type = 0;

	if (iwrite(dip, pos, &de, DIR_ENTRY_SIZE) != DIR_ENTRY_SIZE) {
		warning("cannot iwrite dir new entry");
		return -1;
	}
	if (iwrite(dip, pos + DIR_ENTRY_SIZE, name, len) != len) {
		warning("cannot iwrite entry name string");
		return -1;
	}
	ip->i_nlinks++;
	iupdate(ip);
	return pos;
}
