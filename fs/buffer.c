// https://github.com/archibate/Linux011/blob/master/src/V0.11/fs/buffer.c
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/cli.h>
#include <stddef.h>

extern char end[0];
struct buf *start_buffer = (struct buf *) &end;
struct buf *hash_table[NR_HASH];
static struct buf *free_list;
static struct task *buffer_wait;
int nr_buffers = 0;

int sys_sync(void)
{
	sync_inodes();
	for (int i = 0; i < NR_BUFFERS; i++) {
		struct buf *b = &start_buffer[i];
		if (b->b_dirt)
			ll_rw_block(WRITE, b);
	}
	return 0;
}

int sync_dev(int dev)
{
	for (int i = 0; i < NR_BUFFERS; i++) {
		struct buf *b = &start_buffer[i];
		if (b->b_dev != dev)
			continue;
		if (b->b_dev == dev && b->b_dirt)
			ll_rw_block(WRITE, b);
	}
	sync_inodes();
	for (int i = 0; i < NR_BUFFERS; i++) {
		struct buf *b = &start_buffer[i];
		if (b->b_dev != dev)
			continue;
		if (b->b_dev == dev && b->b_dirt)
			ll_rw_block(WRITE, b);
	}
	return 0;
}

static void invalidate_buffers(int dev)
{
	for (int i = 0; i < NR_BUFFERS; i++) {
		struct buf *b = &start_buffer[i];
		if (b->b_dev != dev)
			continue;
		if (b->b_dev == dev)
			b->b_uptodate = b->b_dirt = 0;
	}
}

#define hashfn(dev, block)   (((dev) ^ (block)) % NR_HASH)
#define hashed(dev, block)   (hash_table[hashfn(dev, block)])
#define hash(b)              hashed((b)->b_dev, (b)->b_blocknr)
#define match(b, dev, block) ((b)->b_dev == (dev) && (b)->b_blocknr == (block))

static void remove_from_queues(struct buf *b)
{
	if (b->b_next)
		b->b_next->b_prev = b->b_prev;
	if (b->b_prev)
		b->b_prev->b_next = b->b_next;
	if (hash(b) != b)
		hash(b) = b->b_next;

	if (!b->b_prev_free || !b->b_next_free)
		panic("free block list corrupted");
	b->b_next_free->b_prev_free = b->b_prev_free;
	b->b_prev_free->b_next_free = b->b_next_free;
	if (free_list == b)
		free_list = b->b_next_free;
}

static void insert_into_queues(struct buf *b)
{
	b->b_next_free = free_list;
	b->b_prev_free = free_list->b_prev_free;
	free_list->b_prev_free->b_next_free = b;
	free_list->b_prev_free = b;
	b->b_prev = b->b_next = NULL;
	if (!b->b_dev)
		return;
	b->b_next = hash(b);
	hash(b) = b;
	b->b_next->b_prev = b;
}

static struct buf *find_buffer(int dev, int block)
{
	for (struct buf *b = hashed(dev, block); b; b = b->b_next) {
		if (match(b, dev, block))
			return b;
	}
	return NULL;
}

struct buf *get_hash_table(int dev, int block)
{
	for (;;) {
		struct buf *b = find_buffer(dev, block);
		if(!b) return NULL;
		b->b_count++;
		if (match(b, dev, block))
			return b;
		b->b_count--;
	}
}

//#define BADNESS(b) (((b)->b_dirt << 1) + (b)->b_lock)
#define BADNESS(b) ((b)->b_dirt << 1)
struct buf *getblk(int dev, int block)
{
	struct buf *b;
repeat:
	b = get_hash_table(dev, block);
	if (b) return b;
	struct buf *p = free_list;
	do {
		if (p->b_count)
			continue;
		if (!b || BADNESS(p) < BADNESS(b)) {
			b = p;
			if (!BADNESS(p))
				break;
		}
	} while ((p = p->b_next_free) != free_list);
	if (!b) {
		sleep_on(&buffer_wait);
		goto repeat;
	}
	if (b->b_count)
		goto repeat;
	while (b->b_dirt) {
		sync_dev(b->b_dev);
		if (b->b_count)
			goto repeat;
	}
	if (find_buffer(dev, block))
		goto repeat;
	b->b_count = 1;
	b->b_dirt = 0;
	b->b_uptodate = 0;
	remove_from_queues(b);
	b->b_dev = dev;
	b->b_blocknr = block;
	insert_into_queues(b);
	return b;
}

void bwrite(struct buf *b)
{
	b->b_dirt = 1;
}

void brelse(struct buf *b)
{
	if (!(b->b_count--))
		panic("trying to free free buffer");
	wake_up(&buffer_wait);
}

struct buf *bread(int dev, int block)
{
	struct buf *b = getblk(dev, block);
	if (!b)
		panic("bread: getblk returned NULL");
	if (b->b_uptodate)
		return b;
	ll_rw_block(READ, b);
	if (b->b_uptodate)
		return b;
	brelse(b);
	return NULL;
}


void init_buffer(unsigned long buffer_end)
{
	struct buf *h = start_buffer;
	void *b;

	if (buffer_end == (1<<20))
		b = (void *)(640*1024);
	else
		b = (void *)buffer_end;
	while ((b -= BLOCK_SIZE) >= (void *)(h + 1)) {
		h->b_dev = 0;
		h->b_dirt = 0;
		h->b_count = 0;
		h->b_uptodate = 0;
		h->b_next = NULL;
		h->b_prev = NULL;
		h->b_data = b;
		h->b_prev_free = h-1;
		h->b_next_free = h+1;
		h++;
		NR_BUFFERS++;
		if (b == (void *)0x100000)
			b = (void *)0xa0000;
	}
	h--;
	free_list = start_buffer;
	free_list->b_prev_free = h; // NR_BUFFERS != 0 assumed
	h->b_next_free = free_list;
}
