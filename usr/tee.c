#include <stdio.h>

int main(int argc, char **argv)
{
	int i;
	char buf[256];
	FILE *out[argc];
	out[0] = stdout;
	for (i = 1; i < argc; i++) {
		out[i] = fopen(argv[i], "w");
		if (!out[i]) {
			perror(argv[i]);
		}
	}
	while (fgets(buf, sizeof(buf), stdin)) {
		for (i = 0; i < argc; i++) {
			if (out[i]) {
				fputs(buf, out[i]);
			}
		}
	}
	return 0;
}
