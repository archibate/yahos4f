#pragma once

#include <stddef.h>
#undef offsetof

void *old_malloc(size_t size);
void *old_calloc(size_t nmemb, size_t size);
void old_free(void *p);
