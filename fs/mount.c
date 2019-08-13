#include <linux/fs.h>
#include <stddef.h>

int mount_super(struct super_block *s, struct inode *ip)
{
	if (!S_ISDIR(ip->i_mode))
		return 0;
	if (s->s_imount || ip->i_mount)
		return 0;
	s->s_imount = ip;
	ip->i_mount = s;
	return 1;
}

int umount_super(struct super_block *s)
{
	if (s->s_imount) {
		s->s_imount->i_mount = NULL;
		s->s_imount = NULL;
		return 1;
	}
}

int umount_inode(struct super_block *ip)
{
	if (ip->i_mount) {
		ip->i_mount->s_imount = NULL;
		ip->i_mount = NULL;
		return 1;
	}
	return 0;
}
