// https://github.com/archibate/Linux011/blob/master/src/V0.11/fs/super.c
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/cli.h>
#include <string.h>

struct super_block super_block[NR_SUPER];

void lock_super(struct super_block *sb)
{
	cli();
	while (sb->s_lock)
		sleep_on(&sb->s_wait);
	sb->s_lock = 1;
	sti();
}

void free_super(struct super_block *sb)
{
	cli();
	sb->s_lock = 0;
	wake_up(&sb->s_wait);
	sti();
}

void wait_on_super(struct super_block *sb)
{
	cli();
	while (sb->s_lock)
		sleep_on(&sb->s_wait);
	sti();
}

struct super_block *get_super(int dev)
{
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

void unload_super(int dev)
{
	if (dev == ROOT_DEV) {
		warning("root diskette changed: prepare for armageddon");
	}
	struct super_block *s = get_super(dev);
	if (!s) return;
	if (s->s_imount) {
		warning("cannot unload mounted disk - tssk, tssk");
		return;
	}
	lock_super(s);
	s->s_dev = 0;
	free_super(s);
}

struct super_block *load_super(int dev)
{
	if (!dev)
		return NULL;
	struct super_block *s = get_super(dev);
	if (s) return s;

	s = get_super(0); // get_free_super
	if (!s) {
		warning("too much super loaded");
		return NULL;
	}

	s->s_dev = dev;
	lock_super(s);
	s->s_isup = NULL;
	s->s_imount = NULL;
	s->s_time = 0;
	s->s_rd_only = 0;
	s->s_dirt = 0;

	struct buf *b = bread(dev, 1);
	if (!b) goto bad;
	memcpy(s, b->b_data, sizeof(struct d_super_block));
	brelse(b);

	if (s->s_magic != SUPER_MAGIC)
		goto bad;
	if (s->s_log_block_size != BLOCK_SIZE_BITS - 10) {
		warning("block size other than 1024 unimpelemented");
		goto bad;
	}

	free_super(s);
	return s;

bad:
	s->s_dev = 0;
	free_super(s);
	return NULL;
}
