#include <linux/kernel.h>
#include <linux/conio.h>
#include <linux/keybd.h>
#include <linux/fs.h>
#include <memory.h>

static size_t tty_write(struct inode *ip, off_t pos, const void *buf,
		size_t size)
{
	for (int i = 0; i < size; i++) {
		cputc(*(const char*)buf++);
	}
	return size;
}

static size_t tty_read(struct inode *ip, off_t pos, void *buf, size_t size)
{
	for (int i = 0; i < size; i++) {
		if ('\n' == (*(char*)buf++ = cgetc()))
			return i + 1;
	}
	return size;
}

static size_t null_write(struct inode *ip, off_t pos, const void *buf,
		size_t size)
{
	return size;
}

static size_t null_read(struct inode *ip, off_t pos, void *buf, size_t size)
{
	return 0;
}

static size_t zero_read(struct inode *ip, off_t pos, void *buf, size_t size)
{
	memset(buf, 0, size);
	return size;
}

struct char_drive drv_table[NR_DRV] = {
	[TTY_DRV] = {
		.read = tty_read,
		.write = tty_write,
	},
	[NULL_DRV] = {
		.read = null_read,
		.write = null_write,
	},
	[ZERO_DRV] = {
		.read = zero_read,
		.write = null_write,
	},
};
