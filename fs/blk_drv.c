#include <linux/fs.h>
#include <string.h>

int blk_read(int dev, int blk, int addr, void *buf, size_t size)
{
	struct buf *b = bread(dev, blk + addr / BLOCK_SIZE);
	if (!b) return 0;
	memcpy(buf, b->b_data + addr % BLOCK_SIZE, size);
	brelse(b);
	return 1;
}

int blk_write(int dev, int blk, int addr, const void *buf, size_t size)
{
	struct buf *b = bread(dev, blk + addr / BLOCK_SIZE);
	if (!b) return 0;
	memcpy(b->b_data + addr % BLOCK_SIZE, buf, size);
	bwrite(b);
	brelse(b);
	return 1;
}
