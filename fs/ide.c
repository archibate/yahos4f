#include <linux/fs.h>
#include <linux/io.h>
#include <linux/fail.h>

/* Status */
#define IDE_BSY     0x80    // Busy
#define IDE_DRDY    0x40    // Ready
#define IDE_DF      0x20    // Write fault
#define IDE_ERR     0x01    // Error

/* Command */
#define IDE_CMD_READ  0x20
#define IDE_CMD_WRITE 0x30
#define IDE_CMD_RDMUL 0xc4
#define IDE_CMD_WRMUL 0xc5

/* Port */
#define IDE_DAT      0x1f0   // word data register (Read-Write)
#define IDE_ERROR    0x1f1   // byte error register (Read)
#define IDE_FEATURE  IDE_ERROR  // byte future register (Write)
#define IDE_SECTNR   0x1f2   // byte secount0 register (Read-Write)
// we use LBA28
#define IDE_LBA0     0x1f3   // byte LAB0 register (Read-Write)
#define IDE_LBA1     0x1f4   // byte LAB1 register (Read-Write)
#define IDE_LBA2     0x1f5   // byte LAB2 register (Read-Write)
#define IDE_CURR     0x1f6   // byte 101dhhhh d=drive hhhh=head (Read-Write)
#define IDE_STAT     0x1f7   // byte status register (Read)
#define IDE_CMD      IDE_STAT // byte status register (Write)
#define IDE_ALTSTAT  0x3f6   // same as IDE_STATUS but doesn't clear IRQ

#define PHYS_BLOCK_SIZE  		512
#define PBPB    (BLOCK_SIZE/PHYS_BLOCK_SIZE)

static void ide_wait(void)
{
	int timeout = 20000;
	int r;

	while ((r = inb(IDE_STAT)) & IDE_BSY) {
		if (timeout-- <= 0)
			fail("ide_wait timeout");
	}

	/*if (r & (IDE_DF|IDE_ERR))
		fail("ide_wait got error");*/
}

static void ide_seek(int dev, int blkno)
{
	ide_wait();

	int lba = blkno * PBPB;

	outb(IDE_SECTNR, PBPB);

	outb(IDE_LBA0, lba         & 0xff);
	outb(IDE_LBA1, (lba >> 8)  & 0xff);
	outb(IDE_LBA2, (lba >> 16) & 0xff);

	unsigned char cur = 0xe0;
	cur |= (dev & 0x1) << 4;
	cur |= (lba >> 24) & 0x0f;
	outb(IDE_CURR, cur);
}

void ide_rw(int ide, int rw, int blkno, void *buf)
{
	ide_seek(ide, blkno);

	outb(IDE_CMD, IDE_CMD_READ);
	ide_wait();
	if (rw == READ) {
		insl(IDE_DAT, buf, BLOCK_SIZE/4);
	} else if (rw == WRITE)
		outsl(IDE_DAT, buf, BLOCK_SIZE/4);
	else
		fail("ide_rw: bad rw command\n");
}
