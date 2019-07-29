#include "api.h"

void usr_echo_main(const char *msg)
{
	int color = 0xf;
	if (msg[0] == '\a') {
		color = 0x8 + (msg[1] - '0');
		msg += 2;
	}
	getchar();
	for (;;) {
		cputs(color, msg);
		wfi();
	}
}
