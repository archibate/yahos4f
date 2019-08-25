#include <unistd.h>
#include <stdio.h>

int main(void)
{
	char buf[11];
	int ret = read(0, buf, sizeof(buf) - 1);
	if (ret <= 0) {
		printf("read returned %d\n", ret);
		return -1;
	}
	buf[ret] = 0;
	printf("read result: %d[%s]\n", ret, buf);
	return 0;
}
