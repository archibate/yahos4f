// https://github.com/archibate/Linux011/blob/master/src/V0.11/fs/super.c
#include <linux/fs.h>
#include <linux/console.h>
#include <linux/sched.h>
#include <linux/fs/super.h>
#include <linux/cli.h>
#include <stddef.h>

struct super_block super_block[NR_SUPER];

static void lock_super(struct super_block *sb)
{
	cli();
	while (sb->s_lock)
		sleep_on(&sb->s_wait);
	sb->s_lock = 1;
	sti();
}

static void free_super(struct super_block *sb)
{
	cli();
	wake_up(&sb->s_wait);
	sti();
}

static void wait_on_super(struct super_block *sb)
{
	cli();
	while (sb->s_lock)
		sleep_on(&sb->s_wait);
	sti();
}

struct super_block *get_super(int dev)
{
	if (!dev)
		return NULL;
again:
	for (int i = 0; i < NR_SUPER; i++) {
		struct super_block *s = &super_block[i];
		if (s->s_dev == dev) {
			wait_on_super(s);
			if (s->s_dev == dev)
				return s;
			goto again;
		}
	}
	return NULL;
}

void put_super(int dev)
{
	if (dev == ROOT_DEV) {
		puts("root diskette changed: prepare for armageddon\n");
	}
	struct super_block *s = get_super(dev);
	if (!s) return;
	if (s->s_imount) {
		puts("mounted disk changed - tssk, tssk\n");
		return;
	}
	lock_super(s);
	s->s_dev = 0;
	for (int i = 0; i < I_MAP_SLOTS; i++)
		brelse(s->s_imap[i]);
	for (int i = 0; i < Z_MAP_SLOTS; i++)
		brelse(s->s_zmap[i]);
	free_super(s);
}

static struct super_block *read_super(int dev)
{
	if (!dev)
		return NULL;
	struct super_block *s = get_super(dev);
	if (s) return s;
	for (int i = 0;; i++) {
		s = &super_block[i];
		if (i >= NR_SUPER)
			return NULL;
		if (!s->s_dev)
			break;
	}
	s->s_dev = dev;
	s->s_isup = NULL;
	s->s_imount = NULL;
	s->s_time = 0;
	s->s_rd_only = 0;
	s->s_dirt = 0;
	lock_super(s);
	struct buf *bh = bread(dev, 1);
	if (!bh)
		goto out_free;
	*(struct d_super_block *)s = *(struct d_super_block *)bh->b_data;
	brelse(bh);
	if (s->s_magic != SUPER_MAGIC)
		goto out_free;
	for (int i = 0; i < I_MAP_SLOTS; i++)
		s->s_imap[i] = NULL;
	for (int i = 0; i < Z_MAP_SLOTS; i++)
		s->s_zmap[i] = NULL;
	int block = 2;
	for (int i = 0; i < s->s_imap_blocks; i++) {
		s->s_imap[i] = bread(dev, block);
		if (!s->s_imap[i])
			break;
		block++;
	}
	for (int i = 0; i < s->s_zmap_blocks; i++) {
		s->s_zmap[i] = bread(dev, block);
		if (!s->s_zmap[i])
			break;
		block++;
	}
	if (block != 2 + s->s_imap_blocks + s->s_zmap_blocks)
		goto out_relse;

	s->s_imap[0]->b_data[0] |= 1;
	s->s_zmap[0]->b_data[0] |= 1;
	goto out;

out_relse:
	for (int i = 0; i < I_MAP_SLOTS; i++)
		brelse(s->s_imap[i]);
	for (int i = 0; i < Z_MAP_SLOTS; i++)
		brelse(s->s_zmap[i]);
out_free:
	s->s_dev = 0;
out:
	free_super(s);
	return NULL;
}
