void usr_echo_main(const char *msg)
{
	for (;;) {
		asm volatile ("int $0x80" :: "a" (1), "c" (0xb), "d" (msg));
		asm volatile ("int $0x80" :: "a" (3));
	}
}
