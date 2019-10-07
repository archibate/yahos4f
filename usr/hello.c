#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
	FILE *fp = fopen("/dev/tty", "w");
	fwrite("Hello, World!\n", 7, 2, fp);
	fclose(fp);
}
