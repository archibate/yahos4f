#include <linux/kernel.h>
#include <linux/fs.h>
#include <string.h>
#include <alloca.h>

int dir_find(struct inode *dip, struct dir_entry *de, const char *na, off_t pos)
{
	char *name = alloca(20);
	size_t last_name_len = 20;
	int ret;

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
	return -pos;
}

int dir_link(struct inode *dip, const char *name, struct inode *ip)
{
	static struct dir_entry de;
	int pos = dir_find(dip, &de, name, 0);
	if (pos >= 0)
		return -pos;
	pos = -pos;

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
