#include "mman.h"
#include "fail.h"

typedef unsigned long size_t;

#define NULL ((void*)0)

typedef struct header
{
	struct header *next, *prev;
	size_t size;
	void *ptr;
} H;

H *first_block;

static H *search_block(H **pprev, size_t size)
{
	*pprev = first_block;
	for (H *b = first_block; b; b = b->next) {
		if (!b->ptr && b->size >= size)
			return b;
		*pprev = b;
	}
	return NULL;
}

static H *extend_heap(H *prev, size_t size)
{
	H *b = sbrk(0);
	if (sbrk(sizeof(H) + size) == (void*)-1)
		return NULL;
	b->size = size;
	b->next = NULL;
	b->prev = prev;
	if (prev)
		prev->next = b;
	return b;
}

static void split_block(H *b, size_t size)
{
	H *c = (void*)(b + 1) + size;
	c->size = b->size - (size + sizeof(H));
	c->next = b->next;
	if (b->next)
		b->next->prev = c;
	c->prev = b;
	b->next = c;
	c->ptr = NULL;
}

static size_t align(size_t s)
{
	return !(s & 7) ? s : ((s >> 3) + 1) << 3;
}

static void *_malloc(size_t size)
{
	H *b;
	size = align(size);
	if (!first_block) {
		b = extend_heap(NULL, size);
		if (!b)
			return NULL;
		first_block = b;
	} else {
		H *prev;
		b = search_block(&prev, size);
		if (!b) {
			b = extend_heap(prev, size);
			if (!b)
				return NULL;
		} else if (b->size >= size + sizeof(H) + 8) {
			split_block(b, size);
		}
	}
	b->ptr = b + 1;
	return b->ptr;
}

void *malloc(size_t size)
{
	char *p = _malloc(size);
	if (!p)
		return NULL;
	for (int i = 0; i < size; i++)
		p[i] = 0xcc;
	return p;
}

void *calloc(size_t nmemb, size_t size)
{
	size *= nmemb;
	char *p = _malloc(size);
	if (!p)
		return NULL;
	for (int i = 0; i < size; i++)
		p[i] = 0;
	return p;
}

void *zalloc(size_t nmemb, size_t size)
{
	size *= nmemb;
	char *p = _malloc(size);
	if (!p)
		return NULL;
	for (int i = 0; i < size; i++)
		p[i] = 0;
	return p;
}

static H *get_block(void *p)
{
	return p - sizeof(H);
}

void merge_block(H *b)
{
	if (b->next && !b->next->ptr) {
		b->size += sizeof(H) + b->next->size;
		b->next = b->next->next;
		if (b->next)
			b->next->prev = b;
	}
}

void free(void *p)
{
	H *b = get_block(p);
	if (!(first_block && (H *)p > first_block + 1 && p < sbrk(0) && p == b->ptr))
		fail("bad free");

	b->ptr = NULL;
	if (b->prev && !b->prev->ptr) {
		b = b->prev;
		merge_block(b);
	}
	if (b->next)
		merge_block(b);
	else {
		if (b->prev)
			b->prev->prev = NULL;
		else
			first_block = NULL;
		brk(b);
	}
}

#if 0 // WORK IN PROGRESS: https://blog.csdn.net/qingzhuyuxian/article/details/80331902
static void *__realloc(void *p, size_t size)
{
	if (!p)
		return malloc(size);
	H *b = get_block(p);
	if (!(first_block && (H *)p > first_block + 1 && p < sbrk(0) && p == b->ptr))
		fail("bad realloc");

	size = align(size);
	if (b->size >= sizeof(H) + 8 + size) {
		split_block(b, size);
	} else {
		if (b->next && !b->next->ptr && b->size + sizeof(H) + b->next->size >= size) {
			merge_block(b);
			if (b->size >= sizeof(H) + 8 + size)
				split_block(b, s);
		} else {
			void *q = malloc(size);
			if (!q)
				return NULL;
			H *c = get_block(q);
			copy_block(b, c);
			free(p);
			return q;
		}
	}
}
#endif
