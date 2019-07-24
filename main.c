void main(void)
{
	short *vram = (short *)0xb8000;
	vram[0] = 0xf00 + 'O';
	vram[1] = 0xf00 + 'K';
}
