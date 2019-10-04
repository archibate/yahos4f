#include <linux/kernel.h>
#include <linux/mman.h>

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

void merge_block(H *b)
{
	if (b->next && !b->next->ptr) {
		if (b == (void*)0x2224b8) printk("?%p+%p %p+%p", b, b->size, b->next, b->next->size);
		b->size += sizeof(H) + b->next->size;
		b->next = b->next->next;
		if (b->next)
			b->next->prev = b;
		if (b == (void*)0x2224b8) printk("@%p+%p %p+%p", b, b->size, b->next, b->next->size);
	}
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
		if (b == (void*)0x2224b8) printk("!%p+%p", b, b->size);
		if (!b) {
			b = extend_heap(prev, size);
			if (!b)
				return NULL;
		} else if (b->size >= size + sizeof(H) + 8) {
			if (b <= b->next && (void*)(b + 1) + b->size > (void*)b->next)
				panic("%p+%p %p+%p", b, b->size, b->next, b->next->size);
			split_block(b, size);
			merge_block(b->next);
		}
	}
	b->ptr = b + 1;
	return b->ptr;
}

void *old_malloc(size_t size)
{
	char *p = _malloc(size);
	if (!p)
		return NULL;
	for (int i = 0; i < size; i++)
		p[i] = 0xcc;
	return p;
}

void *old_calloc(size_t nmemb, size_t size)
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

void old_free(void *p)
{
	H *b = get_block(p);
	if (!(first_block && (H *)p >= first_block + 1
				&& p < sbrk(0) && p == b->ptr))
		panic("bad free %p (%p)", p, b->ptr);
	//return; // TODO: bug may in merge_block

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
		panic("bad realloc");

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
