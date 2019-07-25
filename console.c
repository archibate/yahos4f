#include "console.h"
#include "io.h"

static short *const vram = (short *)0xb8000;
static int cx, cy;
static const int nx = 80, ny = 25;

#define IO_VGA_CRT  0x3d4

static void download_iocur(void)
{
	outb(IO_VGA_CRT+0, 0xe);
	int hi = inb(IO_VGA_CRT+1);
	outb(IO_VGA_CRT+0, 0xf);
	int lo = inb(IO_VGA_CRT+1);
	int off = (hi << 8) | lo;
	cx = off % nx;
	cy = off / nx;
}

static void update_iocur(void)
{
	int off = cy * nx + cx;
	outb(IO_VGA_CRT+0, 0xe);
	outb(IO_VGA_CRT+1, off >> 8);
	outb(IO_VGA_CRT+0, 0xf);
	outb(IO_VGA_CRT+1, off & 0xff);
}

void clear(void)
{
	for (int i = 0; i < ny * nx; i++)
		vram[i] = 0xf00;
	cy = 0;
	cx = 0;
	update_iocur();
}

static void scroll_up(int n)
{
	for (int i = 0; i < (ny - n) * nx; i++)
		vram[i] = vram[i + n * nx];
	for (int i = (ny - n) * nx; i < ny * nx; i++)
		vram[i] = 0xf00;
	cy -= n;
}

int putchar(int c)
{
	if (c == '\n') {
		cx = 0;
		cy++;
		if (cy >= ny) {
			scroll_up(cy - ny + 1);
		}
	} else {
		vram[cy * nx + cx] = 0xf00 + c;
		cx++;
	}
	update_iocur();
}

int puts(const char *s)
{
	while (*s)
		putchar(*s++);
}

