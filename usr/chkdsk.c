#include "api.h"

void usr_chkdsk_main(void)
{
	cputs(0xc, "chkdsk started...\n");

	static char buf[512];
	rdblk(0, 0, buf);
	cputs(0xc, "content of hd 0 lba 0: ");
	cputs(0xd, buf);
	cputs(0xf, "\n");

	for (;;) pause();
}
