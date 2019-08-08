#pragma once

#define READ 0
#define WRITE 1

#define NAME_LEN 14
#define ROOT_INO 2

#define ROOT_DEV root_dev
#define HDA_DEV 1
#define HDB_DEV 2

#define SUPER_MAGIC 0xef53

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
	unsigned int i_size;
	unsigned int i_atime;
	unsigned int i_ctime;
	unsigned int i_mtime;
	unsigned int i_deltime;
	unsigned short i_gid;
	unsigned short i_nlinks;
	unsigned int i_nsectors;
	unsigned int i_flags;
	unsigned int i_os_spec1;
	unsigned int i_zone[11];
	unsigned int i_s_zone;
	unsigned int i_d_zone;
	unsigned int i_t_zone;
	unsigned int i_gen_num;
	unsigned int i_reserved1;
	unsigned int i_reserved2;
	unsigned int i_frag_block;
	unsigned char i_os_spec2[12];
};

struct inode {
	unsigned short i_mode;
	unsigned short i_uid;
	unsigned int i_size;
	unsigned int i_atime;
	unsigned int i_ctime;
	unsigned int i_mtime;
	unsigned int i_deltime;
	unsigned short i_gid;
	unsigned short i_nlinks;
	unsigned int i_nsectors;
	unsigned int i_flags;
	unsigned int i_os_spec1;
	unsigned int i_zone[11];
	unsigned int i_s_zone;
	unsigned int i_d_zone;
	unsigned int i_t_zone;
	unsigned int i_gen_num;
	unsigned int i_reserved1;
	unsigned int i_reserved2;
	unsigned int i_frag_block;
	unsigned char i_os_spec2[12];
/* these are in memory also */
	struct task *i_wait;
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
	unsigned int s_ninodes;
	unsigned int s_nblocks;
	unsigned int s_nblocks_reserved;
	unsigned int s_nblocks_free;
	unsigned int s_ninodes_free;
	unsigned int s_superblock;
	unsigned int s_log_block_size;
	unsigned int s_log_frag_size;
	unsigned int s_blocks_per_group;
	unsigned int s_frags_per_group;
	unsigned int s_inodes_per_group;
	unsigned int s_last_mount_time;
	unsigned int s_last_written_time;
	unsigned short s_nmount_times;
	unsigned short s_nmount_max_times;
	unsigned short s_magic;
	unsigned short s_state;
	unsigned short s_error_handling;
	unsigned short s_minor;
	unsigned int s_ccheck_time;
	unsigned int s_ccheck_interval;
	unsigned int s_os_id;
	unsigned int s_major;
	unsigned short s_reserved_uid;
	unsigned short s_reserved_gid;
/* These are only in memory */
	unsigned short s_dev;
	struct inode *s_isup;
	struct inode *s_imount;
	unsigned int s_time;
	struct task *s_wait;
	unsigned char s_lock;
	unsigned char s_rd_only;
	unsigned char s_dirt;
	unsigned int s_imap_blocks;
};

struct d_super_block {
	unsigned int s_ninodes;
	unsigned int s_nblocks;
	unsigned int s_nblocks_reserved;
	unsigned int s_nblocks_free;
	unsigned int s_ninodes_free;
	unsigned int s_superblock;
	unsigned int s_log_block_size;
	unsigned int s_log_frag_size;
	unsigned int s_blocks_per_group;
	unsigned int s_frags_per_group;
	unsigned int s_inodes_per_group;
	unsigned int s_last_mount_time;
	unsigned int s_last_written_time;
	unsigned short s_nmount_times;
	unsigned short s_nmount_max_times;
	unsigned short s_magic;
	unsigned short s_state;
	unsigned short s_error_handling;
	unsigned short s_minor;
	unsigned int s_ccheck_time;
	unsigned int s_ccheck_interval;
	unsigned int s_os_id;
	unsigned int s_major;
	unsigned short s_reserved_uid;
	unsigned short s_reserved_gid;
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
