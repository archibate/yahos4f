#include <linux/syscall.h>
#include <string.h>

int main(int argc, char **argv, char **envp)
{
	for (int i = 0; envp[i]; i++) {
		_write(1, envp[i], strlen(envp[i]));
		_write(1, "\n", 1);
	}
	return 0;
}
