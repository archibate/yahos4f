#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/cli.h>
#include <string.h>
#include <malloc.h>
#include <stddef.h>
#include <alloca.h>

static struct inode inode[NR_INODE];

void ilock(struct inode *ip)
{
	cli();
	while (ip->i_lock)
		sleep_on(&ip->i_wait);
	ip->i_lock = 1;
	sti();
}

void iunlock(struct inode *ip)
{
	cli();
	ip->i_lock = 0;
	wake_up(&ip->i_wait);
	sti();
}

void iwait(struct inode *ip)
{
	cli();
	while (ip->i_lock)
		sleep_on(&ip->i_wait);
	sti();
}

static struct inode *find_inode(int dev, int ino)
{
again:
	for (int i = 0; i < NR_INODE; i++) {
		struct inode *ip = &inode[i];
		if (ip->i_dev == dev && ip->i_ino == ino) {
			iwait(ip);
			if (ip->i_dev == dev && ip->i_ino == ino)
				return ip;
			goto again;
		}
	}
	return NULL;
}

static struct inode *alloc_inode(void)
{
	for (int i = 0; i < NR_SUPER; i++) {
		struct inode *ip = &inode[i];
		if (!atomic_setifz(&ip->i_count, 1))
			return ip;
	}
	warning("too much super loaded");
	return NULL;
}

struct inode *idup(struct inode *ip)
{
	atomic_add(&ip->i_count, 1);
	return ip;
}

struct inode *iget(int dev, int ino)
{
	struct inode *ip = find_inode(dev, ino);
	if (ip) return idup(ip);

	ip = alloc_inode();
	ilock(ip);
	ip->i_dev = dev;
	ip->i_ino = ino;
	ip->i_dirt = 0;
	ip->i_special = 0;

	void *pi;
	struct buf *b;
	unsigned int addr;
	static struct group_desc gd0;
	struct group_desc *gd = &gd0;
	struct super_block *s = get_super(dev);
	if (!s) {
		warning("superblock not loaded");
		goto bad;
	}

	addr = ((ino - 1) / s->s_inodes_per_group) * GROUP_DESC_SIZE;
	b = bread(dev, 2 + addr / BLOCK_SIZE);
	if (!b) {
		warning("bread failed to read group-desc");
		goto bad;
	}
	pi = b->b_data + addr % BLOCK_SIZE;
	memcpy(gd, pi, sizeof(struct group_desc));
	brelse(b);

	addr = ((ino - 1) % s->s_inodes_per_group) * INODE_SIZE;
	b = bread(dev, gd->itab_block + addr / BLOCK_SIZE);
	if (!b) {
		warning("bread failed to read inode");
		goto bad;
	}
	brelse(b);

	pi = b->b_data + addr % BLOCK_SIZE;
	memcpy(ip, pi, sizeof(struct d_inode));
	iunlock(ip);
	return ip;
bad:
	iunlock(ip);
	atomic_set(&ip->i_count, 0);
	return NULL;
}

void iput(struct inode *ip)
{
	if (atomic_subu(&ip->i_count, 1) < 0)
		warning("iput with i_count <= 0");
}

static int inode_get_zone(struct inode *ip, int zone)
{
	if (zone < NR_DIRECT)
		return ip->i_zone[zone];
	panic("non-direct zone map unimpelemented");
}

static size_t do_irw(struct inode *ip, int rw, off_t pos, void *buf, size_t size)
{
	if (!size) return 0;
	int zone = pos / BLOCK_SIZE;
	int off = pos % BLOCK_SIZE;
	int n = BLOCK_SIZE - off;
	while (size) {
		if (n > size) n = size;

		int block = inode_get_zone(ip, zone);
		struct buf *b = bread(ip->i_dev, block);
		if (!b) return size;
		if (!rw)
			memcpy(buf, b->b_data + off, n);
		else
			memcpy(b->b_data + off, buf, n);
		brelse(b);
		buf += n;
		size -= n;
		n = BLOCK_SIZE;
		off = 0;
		zone++;
	}
	return 0;
}

static size_t iread(struct inode *ip, off_t pos, void *buf, size_t size)
{
	if (pos > ip->i_size) {
		warning("offset out of range");
		return 0;
	}
	if (pos + size > ip->i_size)
		size = ip->i_size - pos;
	size_t rest = do_irw(ip, READ, pos, buf, size);
	return size - rest;
}

static size_t iwrite(struct inode *ip, off_t pos, const void *buf, size_t size)
{
	if (pos > ip->i_size) {
		warning("offset out of range");
		return 0;
	}
	if (pos + size > ip->i_size) {
		panic("file auto extension not impelemented");
		return 0;
	}
	size_t rest = do_irw(ip, WRITE, pos, (void *)buf, size);
	return size - rest;
}

void fs_test(void)
{
	struct super_block *s = load_super(ROOT_DEV);
	if (!s) panic("bad load_super");

	printk("ext2 rev %d.%d", s->s_major, s->s_minor);
	printk("%d/%d inodes", s->s_ninodes - s->s_ninodes_free, s->s_ninodes);
	printk("%d/%d blocks", s->s_nblocks - s->s_nblocks_free, s->s_nblocks);
	printk("");

	struct inode *ip = iget(ROOT_DEV, ROOT_INO);
	if (!ip) panic("bad iget(ROOT_DEV, ROOT_INO)");

	off_t pos = 0;
	static struct dir_entry de;

	while (iread(ip, pos, &de, DIR_ENTRY_SIZE) == DIR_ENTRY_SIZE) {
		char *name = alloca(de.d_name_len + 1);
		de.d_name_len = iread(ip, pos + DIR_ENTRY_SIZE,
				name, de.d_name_len);
		name[de.d_name_len] = 0;
		pos += de.d_entry_size;
		printk("%d %s", de.d_ino, name);
	}
	printk("");
}
