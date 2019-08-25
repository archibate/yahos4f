#include <linux/kernel.h>
#include <linux/conio.h>
#include <linux/keybd.h>
#include <linux/fs.h>

static size_t tty_write(struct inode *ip, const void *buf, size_t size)
{
	for (int i = 0; i < size; i++) {
		cputc(*(const char*)buf++);
	}
	return size;
}

static size_t tty_read(struct inode *ip, void *buf, size_t size)
{
	for (int i = 0; i < size; i++) {
		if ('\n' == (*(char*)buf++ = cgetc()))
			return i;
	}
	return size;
}

struct char_drive drv_table[NR_DRV] = {
	[TTY_DRV] = {
		.read = tty_read,
		.write = tty_write,
	},
};
