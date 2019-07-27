void usr_echo_main(const char *msg)
{
	asm volatile ("int $0x80" :: "a" (1), "c" (0xb), "d" (msg));
	for (;;)
		asm volatile ("int $0x80" :: "a" (2));
}
