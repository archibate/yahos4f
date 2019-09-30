#include <linux/kernel.h>
#include <linux/fs.h>

static const char *stros(int id)
{
	switch (id) {
	case 0: return "Linux";
	case 1: return "GNU/Hurd";
	case 2: return "MASIX";
	case 3: return "FreeBSD";
	case 4: return "BSD4.4-Lite";
	case 233: return "YahOS";
	default: return "Other";
	}
}

void init_fs(void)
{
	struct super_block *s = read_super(ROOT_DEV);
	if (!s) panic("bad read_super(ROOT_DEV)");

	printk("%s ext2 rev %d.%d", stros(s->s_os_id), s->s_major, s->s_minor);
	printk("%d/%d inodes", s->s_ninodes - s->s_ninodes_free, s->s_ninodes);
	printk("%d/%d blocks", s->s_nblocks - s->s_nblocks_free, s->s_nblocks);
	printk("");

	fs_mkdir("/dev", S_IFDIR | 0755);
	fs_mknod("/dev/tty", S_IFCHR | 0644, TTY_DRV);
	fs_mknod("/dev/zero", S_IFCHR | 0644, ZERO_DRV);
	fs_mknod("/dev/null", S_IFCHR | 0644, NULL_DRV);
	fs_mknod("/dev/hda", S_IFBLK | 0644, HDA_DEV);
}
