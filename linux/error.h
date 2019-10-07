#pragma once

#include <errno.h>

#define error_return(errnum, value) do { \
	errno = (errnum); \
	return (value); \
} while (0)
