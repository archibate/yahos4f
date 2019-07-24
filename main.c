int putchar(int c)
{
	short *const vram = (short *)0xb8000;
	static int x, y;
	const int nx = 80, ny = 25;

	if (c == '\n') {
		x = 0;
		y++;
	} else {
		vram[y * nx + x] = 0xf00 + c;
		x++;
	}
}

int puts(const char *s)
{
	while (*s)
		putchar(*s++);
}

void main(void)
{
	puts("Hello, World!\nThis is my Very First OS!\n");
	puts("\nJust do It!\n");
}
