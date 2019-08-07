#include "console.h"
#include "io.h"

static short *const vram = (short *)0xb8000;
static int cx, cy;
static const int nx = 80, ny = 25;
static int color = 0xf00;

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

void setcolor(int clr)
{
	color = clr << 8;
}

int getcolor(void)
{
	return color >> 8;
}

void getcur(int *x, int *y)
{
	if (x) *x = cx;
	if (y) *y = cy;
}

void setcur(int x, int y)
{
	cx = x;
	cy = y;
	update_iocur();
}

void clear(void)
{
	for (int i = 0; i < ny * nx; i++)
		vram[i] = color;
	cy = 0;
	cx = 0;
	update_iocur();
}

static void scroll_up(int n)
{
	for (int i = 0; i < (ny - n) * nx; i++)
		vram[i] = vram[i + n * nx];
	for (int i = (ny - n) * nx; i < ny * nx; i++)
		vram[i] = color;
	cy -= n;
}

int putchar(int c)
{
	if (c == '\n' || cx > nx) {
		cx = 0;
		cy++;
		if (cy >= ny) {
			scroll_up(cy - ny + 1);
		}
	} else if (c == '\r') {
		cx = 0;
	} else {
		vram[cy * nx + cx] = color | c;
		cx++;
	}
	update_iocur();
}

int puts(const char *s)
{
	while (*s)
		putchar(*s++);
}

