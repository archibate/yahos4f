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
	return -1;
}
