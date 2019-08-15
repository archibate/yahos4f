#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/kernel.h>

/* blk_drv/ide.c */
void ide_rw(int ide, int rw, int blkno, void *buf);

void ll_rw_block(int rw, struct buf *b)
{
	if (b->b_dev < 1 || b->b_dev > 2)
		panic("device %d not exist", b->b_dev);
	unsigned ide = b->b_dev - 1;
	if ((rw == WRITE && !b->b_dirt) || (rw == READ && b->b_uptodate))
		return;

	ide_rw(ide, rw, b->b_blocknr, b->b_data);
	if (rw == READ) {
		b->b_uptodate = 1;
	} else if (rw == WRITE) {
		b->b_dirt = 0;
	}
}
