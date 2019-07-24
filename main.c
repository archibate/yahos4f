int putchar(int c)
{
	static short *vram = (short *)0xb8000;
	*vram++ = 0xf00 + c;
}

int puts(const char *s)
{
	while (*s)
		putchar(*s++);
}

void main(void)
{
	puts("Hello, World!");
}
