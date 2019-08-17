#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <string.h>
#include <malloc.h>
#include <stddef.h>

static struct inode inode[NR_INODE];

static struct inode *find_inode(int dev, int ino)
{
	for (int i = 0; i < NR_INODE; i++) {
		struct inode *ip = &inode[i];
		if (ip->i_dev == dev && ip->i_ino == ino)
			return ip;
	}
	return NULL;
}

static struct inode *alloc_inode(void)
{
	for (int i = 0; i < NR_SUPER; i++) {
		struct inode *ip = &inode[i];
		if (!ip->i_count) {
			ip->i_count = 1;
			return ip;
		}
	}
	panic("too much inode loaded, synfree uni");
	return NULL;
}

struct inode *idup(struct inode *ip)
{
	ip->i_count++;
	return ip;
}

void iupdate(struct inode *ip)
{
	if (!blk_write(ip->i_dev, ip->i_on_block,
				ip->i_on_addr, ip, INODE_SIZE)) {
		panic("failed to bwrite inode");
	}
}

static int get_group_desc(struct inode *ip, struct group_desc *gd)
{
	unsigned int addr;
	addr = ((ip->i_ino - 1) / ip->i_sb->s_inodes_per_group) * GROUP_DESC_SIZE;
	if (!blk_read(ip->i_dev, 2, addr, gd, sizeof(struct group_desc))) {
		warning("failed to bread group-desc");
		return 0;
	}
	return 1;
}

static int update_group_desc(struct inode *ip, struct group_desc *gd)
{
	unsigned int addr;
	addr = ((ip->i_ino - 1) / ip->i_sb->s_inodes_per_group) * GROUP_DESC_SIZE;
	if (!blk_write(ip->i_dev, 2, addr, gd, sizeof(struct group_desc))) {
		warning("failed to bwrite group-desc");
		return 0;
	}
	return 1;
}

struct inode *iget(int dev, int ino)
{
	struct inode *ip = find_inode(dev, ino);
	if (ip) return idup(ip);

	ip = alloc_inode();
	ip->i_dev = dev;
	ip->i_ino = ino;

	struct super_block *s = get_super(dev);
	if (!s) {
		warning("device superblock not loaded");
		goto bad;
	}
	ip->i_sb = s;

	static struct group_desc gd;
	if (!get_group_desc(ip, &gd)) {
		warning("failed to get inode group-desc");
		goto bad;
	}

	ip->i_on_addr = ((ino - 1) % s->s_inodes_per_group) * INODE_SIZE;
	ip->i_on_block = gd.itab_block;
	if (!blk_read(dev, ip->i_on_block, ip->i_on_addr, ip, INODE_SIZE)) {
		warning("bread failed to read inode");
		goto bad;
	}

	return ip;
bad:
	ip->i_count = 0;
	return NULL;
}

void iput(struct inode *ip)
{
	if (ip->i_count <= 0) {
		warning("iput with i_count <= 0");
		return;
	}
	ip->i_count--;
}

static void ext2_free_block(struct inode *ip, int block)
{
	static struct group_desc gd;
	if (!get_group_desc(ip, &gd)) {
		warning("failed to get inode group-desc");
		return;
	}
	struct buf *b = bread(ip->i_dev, gd.bmap_block);
	char sel = 1 << (block % 8), *p = &b->b_data[block / 8];
	if (!(*p & sel)) {
		warning("bad free: block %d not allocated", block);
		return;
	}
	gd.nblocks_free++;
	if (!update_group_desc(ip, &gd)) {
		warning("failed to update_group_desc");
		return;
	}
	*p &= ~sel;
	bwrite(b);
	brelse(b);
	ip->i_sb->s_nblocks_free++;
	update_super(ip->i_sb);
}

static int ext2_alloc_block(struct inode *ip, int goal)
{
	int once = 0;
	static struct group_desc gd;
	if (!get_group_desc(ip, &gd)) {
		warning("failed to get inode group-desc");
		return 0;
	}
	struct buf *b = bread(ip->i_dev, gd.bmap_block);
	if (!goal)
		goto oncer;
again:
	for (; goal <= 8 * BLOCK_SIZE; goal++) {
		char sel = 1 << (goal % 8), *p = &b->b_data[goal / 8];
		if (*p & sel)
			continue;
		gd.nblocks_free--;
		if (!update_group_desc(ip, &gd)) {
			warning("failed to update_group_desc");
			return 0;
		}
		*p |= sel;
		bwrite(b);
		brelse(b);
		ip->i_sb->s_ninodes_free--;
		update_super(ip->i_sb);
		return goal;
	}
	if (once) {
		warning("out of block");
		brelse(b);
		return 0;
	}
oncer:
	goal = 1;
	once = 1;
	goto again;
}

static void ext2_free_inode_blocks(struct inode *ip)
{
	for (int i = 0; i < NR_DIRECT; i++) {
		unsigned int *p = &ip->i_zone[i];
		if (!*p) continue;
		ext2_free_block(ip, *p);
		*p = 0;
	}
	if (ip->i_s_zone) {
		struct buf *b = bread(ip->i_dev, ip->i_s_zone);
		for (int i = 0; i < BLOCK_SIZE / sizeof(int); i++) {
			unsigned int *p = &i[(unsigned int*)b->b_data];
			if (!*p) continue;
			ext2_free_block(ip, *p);
			*p = 0;
		}
		bwrite(b);
		brelse(b);
		ext2_free_block(ip, ip->i_s_zone);
		ip->i_s_zone = 0;
	}
	iupdate(ip);
	if (ip->i_d_zone || ip->i_t_zone)
		panic("doubly/triply indirect block unimpelemented");
}

void ext2_free_inode(struct inode *ip)
{
	ext2_free_inode_blocks(ip);
	static struct group_desc gd;
	if (!get_group_desc(ip, &gd)) {
		warning("failed to get inode group-desc");
		return;
	}
	struct buf *b = bread(ip->i_dev, gd.imap_block);
	int ino = ip->i_ino;
	char sel = 1 << ((ino - 1) % 8), *p = &b->b_data[(ino - 1) / 8];
	if (!(*p & sel)) {
		warning("bad free: inode %d not allocated", ino);
		return;
	}
	gd.ninodes_free++;
	if (!update_group_desc(ip, &gd)) {
		warning("failed to update_group_desc");
		return;
	}
	*p &= ~sel;
	bwrite(b);
	brelse(b);
	ip->i_sb->s_ninodes_free++;
	update_super(ip->i_sb);
}

struct inode *ext2_alloc_inode(struct inode *ip)
{
	int once = 0;
	static struct group_desc gd;
	if (!get_group_desc(ip, &gd)) {
		warning("failed to get inode group-desc");
		return NULL;
	}
	struct buf *b = bread(ip->i_dev, gd.imap_block);
	int ino = ip->i_ino;
again:
	for (; ino <= 8 * BLOCK_SIZE; ino++) {
		char sel = 1 << ((ino - 1) % 8), *p = &b->b_data[(ino - 1) / 8];
		if (*p & sel)
			continue;
		gd.ninodes_free--;
		if (!update_group_desc(ip, &gd)) {
			warning("failed to update_group_desc");
			brelse(b);
			return NULL;
		}
		*p |= sel;
		bwrite(b);
		brelse(b);
		ip->i_sb->s_nblocks_free--;
		update_super(ip->i_sb);
		struct inode *rip = alloc_inode();
		rip->i_dev = ip->i_dev;
		rip->i_ino = ino;
		rip->i_sb = ip->i_sb;
		rip->i_on_addr =
			((ino - 1) % ip->i_sb->s_inodes_per_group) * INODE_SIZE;
		rip->i_on_block = gd.itab_block;
		return rip;
	}
	if (once) {
		warning("out of inode");
		brelse(b);
		return NULL;
	}
	ino = 10;
	once = 1;
	goto again;
}

static int ext2_get_singly_block(struct inode *ip, int zone)
{
	if (zone >= BLOCK_SIZE / sizeof(int))
		panic("doubly indirect block not impelemented");

	if (!ip->i_s_zone) {
		ip->i_s_zone = ext2_alloc_block(ip, ip->i_zone[NR_DIRECT-1]);
		if (!ip->i_s_zone) {
			warning("failed to ext2_alloc_block for singly zoner");
			return 0;
		}
		iupdate(ip);
	}

	struct buf *b = bread(ip->i_dev, ip->i_s_zone);
	if (!b) {
		warning("failed to bread singly block zone");
		return 0;
	}
	int block = zone[(int *)b->b_data];
	if (!block) {
		block = ext2_alloc_block(ip,
				zone > 0 ? zone[(int *)b->b_data-1] : 0);
		if (!block) {
			warning("failed to ext2_alloc_block");
			brelse(b);
			return 0;
		}
		zone[(int *)b->b_data] = block;
		bwrite(b);
	}
	brelse(b);
	return block;
}

static int ext2_get_block(struct inode *ip, int zone)
{
	if (zone >= NR_DIRECT)
		return ext2_get_singly_block(ip, zone - NR_DIRECT);

	int block = ip->i_zone[zone];
	if (!block) {
		block = ext2_alloc_block(ip, zone > 0 ? ip->i_zone[zone-1] : 0);
		ip->i_zone[zone] = block;
		iupdate(ip);
		return block;
	}
	return block;
}

static size_t do_irw(struct inode *ip, int rw, off_t pos, void *buf, size_t size)
{
	if (!size) return 0;
	int zone = pos / BLOCK_SIZE;
	int off = pos % BLOCK_SIZE;
	int n = BLOCK_SIZE - off;

	while (size) {
		if (n > size) n = size;

#if 0 // {{{
		if (!s_zb && zone >= NR_DIRECT) {
			if (!ip->i_s_zone) {
				panic("indirect block allocation unimpelemented");
			}
			zb_on_block = ip->i_s_zone;
			zb_on_offset = 0;
			s_zb = bread(ip->i_dev, zb_on_block);
			if (!s_zb) {
				warning("failed to bread singly indirect block");
				break;
			}
			zb = (unsigned int *)s_zb->b_data;
			zone -= NR_DIRECT;
		}
		if (s_zb && zone >= BLOCK_SIZE / sizeof(int)) {
			panic("doubly indirect block unimpelemented");
		}
		block = zb[zone];
		if (!block) {
			b = bread(ip->i_dev, zb_on_block);
			if (!b) {
				warning("failed to bread zb_on_block");
				break;
			}
			unsigned int *p = (unsigned int *)
				(b->b_data + zb_on_offset);
			block = ext2_alloc_block(ip, zone <= 0 ? 0 : zb[zone-1]);
			if (!block) {
				warning("failed to ext2_alloc_block");
				brelse(b);
				break;
			}
			p[zone] = zb[zone] = block;
			bwrite(b);
			brelse(b);
		}
#endif // }}}
		int block = ext2_get_block(ip, zone);
		if (!block) {
			warning("failed to ext2_get_block");
			break;
		}
		struct buf *b = bread(ip->i_dev, block);
		if (!b) {
			warning("failed to bread content block");
			break;
		}
		if (!rw) {
			memcpy(buf, b->b_data + off, n);
		} else {
			memcpy(b->b_data + off, buf, n);
			bwrite(b);
		}
		brelse(b);
		buf += n;
		size -= n;
		n = BLOCK_SIZE;
		off = 0;
		zone++;
	}

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
		ip->i_size = pos + size;
		iupdate(ip);
	}
	size_t rest = do_irw(ip, WRITE, pos, (void *)buf, size);
	return size - rest;
}

void init_inode(struct inode *ip, unsigned int mode)
{
	memset(ip, 0, INODE_SIZE);
	//ip->i_ctime = curr_time;
	//ip->i_atime = curr_time;
	//ip->i_mtime = curr_time;
	ip->i_mode = mode;
}

void sync_inodes(void)
{
}

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

void fs_test(void)
{
	struct super_block *s = read_super(ROOT_DEV);
	if (!s) panic("bad read_super");

	printk("%s ext2 rev %d.%d", stros(s->s_os_id), s->s_major, s->s_minor);
	printk("%d/%d inodes", s->s_ninodes - s->s_ninodes_free, s->s_ninodes);
	printk("%d/%d blocks", s->s_nblocks - s->s_nblocks_free, s->s_nblocks);
	printk("");

	struct inode *ip;
#if 1
	fs_unlink("/dev/simp");
	fs_rmdir("/dev");
	if (fs_mkdir("/dev", S_DFDIR) == -1)
		panic("failed to mkdir for /dev");
	ip = creati("/dev/simp", S_DFREG);
	if (!ip)
		panic("failed to creati for /dev/simp");

	static char str[] = "Hello, S_IFREG!";
	if (iwrite(ip, 0, str, sizeof(str) - 1) != sizeof(str) - 1)
		panic("failed to write for /dev/simp");

	iput(ip);
#endif

	static char buf[233];
	ip = namei("/dev/simp");
	if (!ip)
		panic("failed to namei for /dev/simp");
	buf[iread(ip, 0, buf, sizeof(buf) - 1)] = 0;
	iput(ip);
	printk("%s", buf);
}
