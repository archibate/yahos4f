void usr_echo_main(const char *msg)
{
	int color = 0xf;
	if (msg[0] == '\a') {
		color = 0x8 + (msg[1] - '0');
		msg += 2;
	}
	asm volatile ("int $0x80" :: "a" (4));
	for (;;) {
		asm volatile ("int $0x80" :: "a" (1), "c" (color), "d" (msg));
		asm volatile ("int $0x80" :: "a" (3));
	}
}
