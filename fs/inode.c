#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/cli.h>
#include <stddef.h>
#include <malloc.h>

static struct inode *find_free_inode(void)
{
	return malloc(sizeof(struct inode));//T
}

struct inode *iget(int dev, int ino)
{
	void *pi;
	struct buf *b;
	unsigned int addr;
	struct inode *ip;
	static struct group_desc gd0;
	struct group_desc *gd = &gd0;
	struct super_block *s = get_super(dev);
	if (!s) {
		printk("iget: superblock not loaded");
		goto out;
	}

	addr = ((ino - 1) / s->s_inodes_per_group) * GROUP_DESC_SIZE;
	b = bread(dev, 2 + addr / BLOCK_SIZE);
	if (!b) {
		printk("iget: bread fail to read group-desc");
		goto out;
	}
	pi = b->b_data + addr % BLOCK_SIZE;
	*gd = *(struct group_desc *)pi;
	brelse(b);

	addr = ((ino - 1) % s->s_inodes_per_group) * INODE_SIZE;
	b = bread(dev, gd->itab_block + addr / BLOCK_SIZE);
	if (!b) {
		printk("iget: bread fail to read inode");
		goto out;
	}
	brelse(b);
	ip = find_free_inode();
	pi = b->b_data + addr % BLOCK_SIZE;
	*(struct d_inode *)ip = *(struct d_inode *)pi;
	return ip;
out:
	return NULL;
}

void fs_test(void)
{
	struct super_block *s = load_super(ROOT_DEV);
	if (!s) fail("bad load_super");
	printk("root dev ext2 rev %d.%d", s->s_major, s->s_minor);
	printk("%d/%d inodes", s->s_ninodes - s->s_ninodes_free, s->s_ninodes);
	printk("%d/%d blocks", s->s_nblocks - s->s_nblocks_free, s->s_nblocks);

	struct inode *ip = iget(ROOT_DEV, ROOT_INO);
	if (!ip) fail("bad iget");

	printk("root inode size %d, at %d", ip->i_size, ip->i_zone[0]);
	fail("OK");
}
