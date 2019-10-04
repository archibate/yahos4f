#include <unistd.h>
#include <string.h>

int main(int argc, char **argv, char **envp)
{
	if (argv[1]) {
		execvp(argv[1], argv + 1);
		return -1;
	} else for (int i = 0; envp[i]; i++) {
		write(1, envp[i], strlen(envp[i]));
		write(1, "\n", 1);
	}
	return 0;
}
