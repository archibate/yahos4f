#include <linux/syscall.h>
#include <linux/fs.h>

int main(void)
{
	if (_mkdir("/root", S_DFDIR) == -1)
		_debug("mkdir /root failed!");
	if (_link("/etc/issue", "/root/issue") == -1)
		_debug("link /etc/issue -> /root/issue failed!");
	if (_rmdir("/root") == -1)
		_debug("rmdir /root failed!");
	return 0;
}
