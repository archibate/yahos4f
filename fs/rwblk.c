#include <linux/fs.h>
#include <linux/cli.h>
#include <linux/sched.h>
#include <linux/console.h>

void ide_rw(int ide, int rw, int blkno, void *buf);

static inline void lock_buffer(struct buf *b)
{
	cli();
	while (b->b_lock)
		sleep_on(&b->b_wait);
	b->b_lock = 1;
	sti();
}

static inline void unlock_buffer(struct buf *b)
{
	if (!b->b_lock)
		puts("rwblk.c: buffer not locked\n");
	b->b_lock = 0;
	wake_up(&b->b_wait);
}

void ll_rw_block(int rw, struct buf *b)
{
	unsigned ide = b->b_dev - 1;
	lock_buffer(b);
	if ((rw == WRITE && !b->b_dirt) || (rw == READ && b->b_uptodate))
		goto out_unlock;
repeat:
	ide_rw(ide, rw, b->b_blocknr, b->b_data);
	if (rw == READ) {
		b->b_uptodate = 1;
	} else if (rw == WRITE) {
		b->b_dirt = 0;
	}
out_unlock:
	unlock_buffer(b);
	return;
}
