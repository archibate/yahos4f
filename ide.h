#pragma once

typedef unsigned char dev_t;
typedef unsigned int blkno_t;

#define BSIZE   512
#define PBSIZE  512
#define PBPB    (BSIZE/PBSIZE)

void ide_wrblk(dev_t dev, blkno_t blkno, const void *buf);
void ide_rdblk(dev_t dev, blkno_t blkno, void *buf);
