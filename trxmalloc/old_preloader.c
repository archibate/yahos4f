/*
 * Authors:
 * + Andrea Fioraldi <andreafioraldi@gmail.com>
 * + Pietro Borrello <pietro.borrello95@gmail.com>
 *
 * License: BSD 2-Clause
 */

#include "old_malloc.h"

void *malloc(size_t size) {
  return old_malloc(size);
}

void free(void *ptr) {
  old_free(ptr);
}

void *calloc(size_t nmemb, size_t size) {
  return old_calloc(nmemb, size);
}
