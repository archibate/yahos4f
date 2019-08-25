#include <linux/kernel.h>
#include <linux/fs.h>
#include <string.h>
#include <alloca.h>

int dir_find(struct inode *dip, struct dir_entry *de, const char *na, off_t pos)
{
	char *name = alloca(20);
	size_t last_name_len = 20;
	int ret, i;

	de->d_entry_size = 0;

	if (!S_ISDIR(dip->i_mode))
		return -1; /* ENOTDIR */

	for (i = 0; iread(dip, pos, de, DIR_ENTRY_SIZE) == DIR_ENTRY_SIZE; i++) {
		if (!de->d_ino) {
			pos += de->d_entry_size;
			continue;
		}
		if (de->d_name_len >= last_name_len) {
			name = alloca(de->d_name_len + 1);
			last_name_len = de->d_name_len;
		}
		de->d_name_len = iread(dip, pos + DIR_ENTRY_SIZE,
				name, de->d_name_len);
		name[de->d_name_len] = 0;
		if (!strcmp(na, name))
			return pos;
		pos += de->d_entry_size;
	}
	if (i == 0) {
		warning("dir even with out [.] entry");
		return -1; /* EIO */
	}
	return -1 - pos;
}

int dir_destroy(struct inode *dip)
{
	if (!S_ISDIR(dip->i_mode))
		return 0;

	int pos = 0, i;
	char name[2];
	struct dir_entry de;
	struct inode *ip[2];
	for (i = 0; i < 2; i++) {
		if (iread(dip, pos, &de, DIR_ENTRY_SIZE) != DIR_ENTRY_SIZE) {
			warning("dir with out [%s] entry", i ? ".." : ".");
			return -1; /* EIO */
		}
		if (de.d_name_len != 1 + i) {
			warning("dir with bad [%s] entry", i ? ".." : ".");
			return -1; /* EIO */
		}
		if (!i && de.d_ino != dip->i_ino) {
			warning("dir with bad [.] entry");
			return -1; /* EIO */
		}
		ip[i] = iget(dip->i_dev, de.d_ino);
		if (!ip) {
			warning("failed to iget for [%s] entry", i ? ".." : ".");
			if (i) iput(ip[0]);
			return -1; /* EIO */
		}
		iread(dip, pos + DIR_ENTRY_SIZE, name, 1 + i);
		if (name[0] != '.') {
			warning("dir with bad [%s] entry", i ? ".." : ".");
			return -1; /* EIO */
		}
		if (i && name[1] != '.') {
			warning("dir with bad [..] entry");
			return -1; /* EIO */
		}
		pos += de.d_entry_size;
	}
	while (iread(dip, pos, &de, DIR_ENTRY_SIZE) == DIR_ENTRY_SIZE) {
		if (de.d_ino) {
			//warning("dir_not_empty");
			return -1; /* ENOTEMPTY */
		}
		pos += de.d_entry_size;
	}
	for (i = 0; i < 2; i++) {
		ip[i]->i_nlinks--;
		iupdate(ip[i]);
		iput(ip[i]);
	}
	return 0;
}

int dir_unlink(struct inode *dip, const char *name, int rd)
{
	if (!S_ISDIR(dip->i_mode))
		return -1; /* ENOTDIR */

	static struct dir_entry de, deo;
	int pos = dir_find(dip, &de, name, 0);
	if (pos < 0)
		return -1; /* ENOENT */
#if 0 // {{{
	if (!pos) return -1;
	pos -= de.d_entry_size;
	if (iread(dip, pos, &deo, DIR_ENTRY_SIZE) != DIR_ENTRY_SIZE) {
		warning("cannot iread dir old prev entry");
		return -1;
	}
	if (iwrite(dip, pos, &deo, DIR_ENTRY_SIZE) != DIR_ENTRY_SIZE) {
		warning("cannot iread dir old prev entry");
		return -1;
	}
#endif // }}}

	int ino = de.d_ino;
	struct inode *ip = iget(dip->i_dev, ino);
	if (!ip) {
		warning("iget returned NULL");
		return -1; /* EIO */
	}
	if (rd == 1) {
		if (!S_ISDIR(ip->i_mode)) {
			iput(ip);
			return -1; /* ENOTDIR */
		}
	} else if (rd == 0 && S_ISDIR(ip->i_mode)) {
		iput(ip);
		return -1; /* EISDIR */
	}
	if (dir_destroy(ip) == -1)
		return -1;

	size_t len = de.d_name_len;
	de.d_ino = 0;
	de.d_name_len = 0;
	de.d_type = 0;

	if (iwrite(dip, pos, &de, DIR_ENTRY_SIZE) != DIR_ENTRY_SIZE) {
		warning("cannot iwrite dir del entry");
		iput(ip);
		return -1; /* EIO */
	}
	char *zero = alloca(len);
	if (iwrite(dip, pos + DIR_ENTRY_SIZE, zero, len) != len) {
		warning("cannot iwrite zero entry name string");
		iput(ip);
		return -1; /* EIO */
	}
	if (--ip->i_nlinks <= 0) {
		ext2_free_inode(ip);
	}
	iupdate(ip);
	iput(ip);
	return 0;
}

int dir_link(struct inode *dip, const char *name, struct inode *ip)
{
	if (!S_ISDIR(dip->i_mode))
		return -1; /* ENOTDIR */
	if (dip->i_dev != ip->i_dev)
		return -1; /* EXDEV */

	static struct dir_entry de;
	int pos = dir_find(dip, &de, name, 0);
	if (pos >= 0)
		return -1; /* EEXIST */
	pos = -1 - pos;
	if (!pos) return -1; /* EEXIST */
	pos -= de.d_entry_size;

	size_t min_entry_size = de.d_entry_size;
	de.d_entry_size = ((pos + DIR_ENTRY_SIZE + de.d_name_len + 4) & ~3) - pos;
	if (iwrite(dip, pos, &de, DIR_ENTRY_SIZE) != DIR_ENTRY_SIZE) {
		warning("cannot iwrite dir old prev entry");
		return -1; /* EIO */
	}
	pos += de.d_entry_size;
	min_entry_size -= de.d_entry_size;

	size_t len = strlen(name);
	de.d_ino = ip->i_ino;
	de.d_name_len = len;
	de.d_entry_size = sizeof(de) + len;
	if (de.d_entry_size <= min_entry_size)
		de.d_entry_size = min_entry_size;
	else
		panic("directory contents bigger than a block uni");
	de.d_type = 0;

	if (iwrite(dip, pos, &de, DIR_ENTRY_SIZE) != DIR_ENTRY_SIZE) {
		warning("cannot iwrite dir new entry");
		return -1; /* EIO */
	}
	if (iwrite(dip, pos + DIR_ENTRY_SIZE, name, len) != len) {
		warning("cannot iwrite entry name string");
		return -1; /* EIO */
	}
	ip->i_nlinks++;
	iupdate(ip);
	return pos;
}

int dir_init(struct inode *dip, struct inode *pip)
{
	if (!S_ISDIR(dip->i_mode))
		return -1; /* ENOTDIR */

	static struct dir_entry de;
	de.d_ino = dip->i_ino;
	de.d_entry_size = DIR_ENTRY_SIZE + 4;
	de.d_name_len = 1;
	de.d_type = 0;

	off_t pos = 0;
	if (iwrite(dip, pos, &de, DIR_ENTRY_SIZE) != DIR_ENTRY_SIZE) {
		warning("cannot iwrite dir first entry");
		return -1;
	}
	dip->i_nlinks++;
	pos += DIR_ENTRY_SIZE;
	if (iwrite(dip, pos, ".\0\0\0", 4) != 4) {
		warning("cannot iwrite dir first entry name");
		return -1;
	}
	pos += 4;

	de.d_ino = pip->i_ino;
	de.d_name_len = 2;
	de.d_entry_size = BLOCK_SIZE - (DIR_ENTRY_SIZE + 4);
	if (iwrite(dip, pos, &de, DIR_ENTRY_SIZE) != DIR_ENTRY_SIZE) {
		warning("cannot iwrite dir second entry");
		return -1;
	}
	pip->i_nlinks++;
	pos += DIR_ENTRY_SIZE;
	if (iwrite(dip, pos, "..\0\0", 4) != 4) {
		warning("cannot iwrite dir second entry name");
		return -1;
	}
	pos += 4;
	char *zero = alloca(BLOCK_SIZE - pos);
	memset(zero, 0, BLOCK_SIZE - pos);
	if (iwrite(dip, pos, zero, BLOCK_SIZE - pos) != BLOCK_SIZE - pos) {
		warning("cannot iwrite fill zero for dir block");
		return -1;
	}
	return 0;
}

struct inode *dir_creat(struct inode *dip, const char *name, unsigned int mode,
		unsigned int nod)
{
	if (!S_ISDIR(dip->i_mode))
		return NULL; /* ENOTDIR */

	struct inode *ip = ext2_alloc_inode(dip);
	if (!ip) {
		warning("cannot ext2_alloc_inode");
		return NULL;
	}
	init_inode(ip, mode, nod);

	if (dir_link(dip, name, ip) < 0) {
		ext2_free_inode(ip);
		iput(ip);
		return NULL;
	}
	iupdate(ip);
	return ip;
}
