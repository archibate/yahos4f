#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/cli.h>
#include <string.h>
#include <malloc.h>
#include <stddef.h>

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

void iupdate(struct inode *ip)
{
	struct buf *b = bread(ip->i_dev, ip->i_on_block);
	memcpy(b->b_data + ip->i_on_offset, ip, INODE_SIZE);
	brelse(b);
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
	ip->i_special_type = 0;

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
		warning("failed to bread group-desc");
		return 0;
	}
	pi = b->b_data + addr % BLOCK_SIZE;
	memcpy(gd, pi, sizeof(struct group_desc));
	brelse(b);

	addr = ((ino - 1) % s->s_inodes_per_group) * INODE_SIZE;
	ip->i_on_block = gd->itab_block + addr / BLOCK_SIZE;
	ip->i_on_offset = addr % BLOCK_SIZE;
	b = bread(dev, ip->i_on_block);
	if (!b) {
		warning("bread failed to read inode");
		goto bad;
	}
	brelse(b);

	pi = b->b_data + ip->i_on_offset;
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

static int inode_get_zone(struct inode *ip, int i)
{
	if (i < NR_DIRECT)
		return ip->i_zone[i];
	panic("non-direct zone map unimpelemented");
}

#if 0
static void alloc_block(struct group_desc *gd)
{
	unsigned int addr = gd->bmap_block;
}

static void do_extension(struct inode *ip, size_t size)
{
	int m = (ip->i_size + BLOCK_SIZE - 1) / BLOCK_SIZE;
	int n = (      size + BLOCK_SIZE - 1) / BLOCK_SIZE;
	if (m <= n)
		return;

	if (n >= NR_DIRECT)
		panic("non-direct zone map unimpelemented");

	static struct group_desc gd0;
	struct group_desc *gd = &gd0;

	get_inode_group_desc(gd);
	for (int i = m; i < n; i++) {
		if (!ip->i_zone[i]) {
			ip->i_zone[i] = alloc_block(gd);
		}
	}
	ip->i_size = size;
}
#endif

static size_t do_irw(struct inode *ip, int rw, off_t pos, void *buf, size_t size)
{
	if (!size) return 0;
	int zone = pos / BLOCK_SIZE;
	int off = pos % BLOCK_SIZE;
	int n = BLOCK_SIZE - off;
	unsigned int *zb = ip->i_zone;
	struct buf *s_zb = NULL;

	while (size) {
		if (n > size) n = size;

		if (!s_zb && zone >= NR_DIRECT) {
			s_zb = bread(ip->i_dev, ip->i_s_zone);
			if (!s_zb) {
				warning("failed to bread singly indirect block");
				break;
			}
			zb = (unsigned int *)s_zb->b_data;
			zone -= NR_DIRECT;
		}
		if (s_zb && zone >= BLOCK_SIZE / sizeof(int)) {
			panic("doubly indirect blockunimpelemented");
		}
		int block = zb[zone];
		struct buf *b = bread(ip->i_dev, block);
		if (!b) break;
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
	if (s_zb)
		brelse(s_zb);
	return size;
}

size_t iread(struct inode *ip, off_t pos, void *buf, size_t size)
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

size_t iwrite(struct inode *ip, off_t pos, const void *buf, size_t size)
{
	if (pos > ip->i_size) {
		warning("offset out of range");
		return 0;
	}
	if (pos + size > ip->i_size) {
		inform("file auto extension is an experimental feature");
		ip->i_size = pos + size;
		iupdate(ip);
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

	struct inode *ip = namei("/etc/issue");
	static char buf[256];
	buf[iread(ip, 0, buf, sizeof(buf) - 1)] = 0;
	printk("%s", buf);
	iput(ip);

	printk("");
}
