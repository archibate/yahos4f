// https://github.com/archibate/Linux011/blob/master/src/V0.11/fs/super.c
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <string.h>

struct super_block super_block[NR_SUPER];

struct super_block *get_super(int dev)
{
	for (int i = 0; i < NR_SUPER; i++) {
		struct super_block *s = &super_block[i];
		if (s->s_dev == dev)
			return s;
	}
	return NULL;
}

void put_super(int dev)
{
	if (dev == ROOT_DEV)
		warning("root diskette changed: prepare for armageddon");
	struct super_block *s = get_super(dev);
	if (!s) return;
	if (s->s_imount) {
		warning("cannot unload mounted disk - tssk, tssk");
		return;
	}
	s->s_dev = 0;
}

struct super_block *read_super(int dev)
{
	if (!dev)
		return NULL;
	struct super_block *s = get_super(dev);
	if (s) return s;

	s = get_super(0); // get_free_super()
	if (!s) {
		warning("too much super loaded");
		return NULL;
	}

	s->s_dev = dev;
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

	return s;

bad:
	s->s_dev = 0;
	return NULL;
}
