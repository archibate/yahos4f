#pragma once

#define READ 0
#define WRITE 1

#define NAME_LEN 14
#define ROOT_INO 1
#define ROOT_DEV root_dev

#define HDA_DEV 1
#define HDB_DEV 2

#define I_MAP_SLOTS 8
#define Z_MAP_SLOTS 8
#define SUPER_MAGIC 0x137f

#define NR_OPEN 20
#define NR_INODE 32
#define NR_FILE 64
#define NR_SUPER 8
#define NR_HASH 307
#define NR_BUFFERS nr_buffers
#define BLOCK_SIZE 1024
#define BLOCK_SIZE_BITS 10

#define INODES_PER_BLOCK ((BLOCK_SIZE)/(sizeof (struct d_inode)))
#define DIR_ENTRIES_PER_BLOCK ((BLOCK_SIZE)/(sizeof (struct dir_entry)))

struct buf {
	char *b_data;			/* pointer to data block (1024 bytes) */
	unsigned long b_blocknr;	/* block number */
	unsigned short b_dev;		/* device (0 = free) */
	unsigned char b_uptodate;
	unsigned char b_dirt;		/* 0-clean,1-dirty */
	unsigned char b_count;		/* users using this block */
	unsigned char b_lock;		/* 0 - ok, 1 -locked */
	struct task *b_wait;
	struct buf *b_prev;
	struct buf *b_next;
	struct buf *b_prev_free;
	struct buf *b_next_free;
};

struct d_inode {
	unsigned short i_mode;
	unsigned short i_uid;
	unsigned long i_size;
	unsigned long i_time;
	unsigned char i_gid;
	unsigned char i_nlinks;
	unsigned short i_zone[9];
};

struct inode {
	unsigned short i_mode;
	unsigned short i_uid;
	unsigned long i_size;
	unsigned long i_mtime;
	unsigned char i_gid;
	unsigned char i_nlinks;
	unsigned short i_zone[9];
/* these are in memory also */
	struct task_struct * i_wait;
	unsigned long i_atime;
	unsigned long i_ctime;
	unsigned short i_dev;
	unsigned short i_num;
	unsigned short i_count;
	unsigned char i_lock;
	unsigned char i_dirt;
	unsigned char i_pipe;
	unsigned char i_mount;
	unsigned char i_seek;
	unsigned char i_update;
};

typedef long off_t;
struct file {
	unsigned short f_mode;
	unsigned short f_flags;
	unsigned short f_count;
	struct inode *f_inode;
	off_t f_pos;
};

struct super_block {
	unsigned short s_ninodes;
	unsigned short s_nzones;
	unsigned short s_imap_blocks;
	unsigned short s_zmap_blocks;
	unsigned short s_firstdatazone;
	unsigned short s_log_zone_size;
	unsigned long s_max_size;
	unsigned short s_magic;
/* These are only in memory */
	struct buf *s_imap[8];
	struct buf *s_zmap[8];
	unsigned short s_dev;
	struct inode *s_isup;
	struct inode *s_imount;
	unsigned long s_time;
	struct task *s_wait;
	unsigned char s_lock;
	unsigned char s_rd_only;
	unsigned char s_dirt;
};

struct d_super_block {
	unsigned short s_ninodes;
	unsigned short s_nzones;
	unsigned short s_imap_blocks;
	unsigned short s_zmap_blocks;
	unsigned short s_firstdatazone;
	unsigned short s_log_zone_size;
	unsigned int s_max_size;
	unsigned short s_magic;
};

struct dir_entry {
	unsigned short inode;
	char name[NAME_LEN];
};

extern struct inode inode_table[NR_INODE];
extern struct file file_table[NR_FILE];
extern struct super_block super_block[NR_SUPER];
extern struct buf *start_buffer;
extern int nr_buffers;
extern int root_dev;

void init_buffer(unsigned long buffer_end);
void ll_rw_block(int rw, struct buf *b);
struct buf *bread(int dev, int block);
void brelse(struct buf *b);
