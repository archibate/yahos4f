#pragma once

struct fifo
{
	unsigned char wr, rd;
	char buf[256];
};

int fifo_full(struct fifo *f)
{
	return f->wr == f->rd - 1;
}

int fifo_empty(struct fifo *f)
{
	return f->wr == f->rd;
}

void fifo_put(struct fifo *f, int c)
{
	f->buf[f->wr++] = c;
}

int fifo_get(struct fifo *f)
{
	return f->buf[f->rd++];
}
