#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <malloc.h>
#include <limits.h>

extern int xvasprintf(char * buf, const char * fmt, va_list args);

int vasprintf(char ** buf, const char * fmt, va_list args) {
	char * b = malloc(1024);
	*buf = b;
	return xvasprintf(b, fmt, args);
}

int vfprintf(FILE * device, const char *fmt, va_list args) {
	char * buffer;
	vasprintf(&buffer, fmt, args);

	int out = fwrite(buffer, 1, strlen(buffer), device);
	free(buffer);
	return out;
}

int vprintf(const char *fmt, va_list args) {
	return vfprintf(stdout, fmt, args);
}

int fprintf(FILE * device, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	char * buffer;
	vasprintf(&buffer, fmt, args);
	va_end(args);

	int out = fwrite(buffer, 1, strlen(buffer), device);
	free(buffer);
	return out;
}

int printf(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	char * buffer;
	vasprintf(&buffer, fmt, args);
	va_end(args);
	int out = fwrite(buffer, 1, strlen(buffer), stdout);
	free(buffer);
	return out;
}

