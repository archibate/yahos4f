#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv, char **envp)
{
	if (argv[1]) {
		execvp(argv[1], argv + 1);
		return -1;
	} else for (int i = 0; envp[i]; i++) {
		puts(envp[i]);
	}
	return 0;
}
