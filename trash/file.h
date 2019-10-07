#pragma once

#include <stdio.h>

struct _IO_FILE
{
	int fd;
	int pos, is, bk, ed;
	char *buf;
};
