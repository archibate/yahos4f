#include <stdio.h>

void cat(const char *path)
{
	FILE *fp = path ? fopen(path, "r") : stdin;
	if (!fp) {
		perror(path);
		return;
	}

	char buf[256];
	while (fgets(buf, sizeof(buf), fp)) {
		fputs(buf, stdout);
	}

	if (fp != stdin) fclose(fp);
}

int main(int argc, char **argv)
{
	if (argc == 1) cat(NULL);
	else for (int i = 1; i < argc; i++)
		cat(argv[i]);
	return 0;
}
