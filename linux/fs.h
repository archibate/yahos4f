#pragma once

#include <linux/atomic.h>
#include <sys/types.h>

#define READ 0
#define WRITE 1

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
#define NR_DIRECT 12
#define BLOCK_SIZE 1024
#define BLOCK_SIZE_BITS 10

#define INODE_SIZE		sizeof(struct d_inode)
#define DIR_ENTRY_SIZE		sizeof(struct dir_entry)
#define GROUP_DESC_SIZE		sizeof(struct group_desc)

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
	unsigned int i_zone[NR_DIRECT];
	unsigned int i_s_zone;
	unsigned int i_d_zone;
	unsigned int i_t_zone;
	unsigned int i_gen_num;
	unsigned int i_reserved1;
	unsigned int i_reserved2;
	unsigned int i_frag_block;
	unsigned char i_os_spec2[12];
};

#define S_ISUID	04000
#define S_ISGID	02000
#define S_ISVTX	01000
#define S_RWXU	0700
#define S_RUSR	0400
#define S_WUSR	0200
#define S_XUSR	0100
#define S_RWXG	070
#define S_RGRP	040
#define S_WGRP	020
#define S_XGRP	010
#define S_RWXO	07
#define S_ROTH	04
#define S_WOTH	02
#define S_XOTH	01

#define S_IFMT		0xf000
#define S_IFIFO		0x1000
#define S_IFDIR		0x4000
#define S_IFCHR		0x2000
#define S_IFBLK		0x6000
#define S_IFREG		0x8000
#define S_IFLNK		0xa000
#define S_IFSOCK	0xc000

#define S_ISFIFO(m)	((m & S_IFMT) == S_IFIFO)
#define S_ISDIR(m)	((m & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)	((m & S_IFMT) == S_IFCHR)
#define S_ISBLK(m)	((m & S_IFMT) == S_IFBLK)
#define S_ISREG(m)	((m & S_IFMT) == S_IFREG)
#define S_ISLNK(m)	((m & S_IFMT) == S_IFLNK)
#define S_ISSOCK(m)	((m & S_IFMT) == S_IFSOCK)

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
	unsigned int i_zone[NR_DIRECT];
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
	struct super_block *i_sb;
	struct super_block *i_mount;
	unsigned int i_dev;
	unsigned int i_ino;
	atomic_t i_count;
	unsigned char i_lock;
	unsigned char i_dirt;
	unsigned short i_reserved;
	unsigned int i_on_block;
	unsigned int i_on_addr;
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
	unsigned int s_dev;
	struct inode *s_isup;
	struct inode *s_imount;
	unsigned int s_time;
	struct task *s_wait;
	unsigned char s_lock;
	unsigned char s_rd_only;
	unsigned char s_dirt;
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

struct group_desc {
	unsigned int bmap_block;
	unsigned int imap_block;
	unsigned int itab_block;
	unsigned short nblocks_free;
	unsigned short ninodes_free;
	unsigned short ndirectories;
	unsigned char unused[14];
};

struct dir_entry {
	unsigned int d_ino;
	unsigned short d_entry_size;
	unsigned char d_name_len;
	unsigned char d_type;
};

extern struct inode inode_table[NR_INODE];
extern struct file file_table[NR_FILE];
extern struct super_block super_block[NR_SUPER];
extern struct buf *start_buffer;
extern int nr_buffers;
extern int root_dev;

// fs/rwblk.c
void ll_rw_block(int rw, struct buf *b);

// fs/buffer.c
void init_buffer(unsigned long buffer_end);
struct buf *bread(int dev, int block);
void brelse(struct buf *b);

// fs/super.c
void lock_super(struct super_block *sb);
void free_super(struct super_block *sb);
void wait_on_super(struct super_block *sb);
struct super_block *get_super(int dev);
struct super_block *load_super(int dev);
void unload_super(int dev);

// fs/inode.c
void ilock(struct inode *ip);
void iunlock(struct inode *ip);
void iwait(struct inode *ip);
struct inode *iget(int dev, int ino);
struct inode *idup(struct inode *ip);
size_t iread(struct inode *ip, off_t pos, void *buf, size_t size);
size_t iwrite(struct inode *ip, off_t pos, const void *buf, size_t size);
void iupdate(struct inode *ip);
void iput(struct inode *ip);

// fs/dir.c
int dir_find(struct inode *dip, struct dir_entry *de, const char *na, off_t pos);
int dir_link(struct inode *dip, const char *name, struct inode *ip);

// fs/path.c
struct inode *dir_geti(struct inode *dip, const char *path);
struct inode *dir_getp(struct inode *dip, const char **ppath);
int dir_linki(struct inode *dip, const char *path, struct inode *ip);

// fs/namei.c
struct inode *namei(const char *path);
struct inode *namep(const char **path);
int linki(const char *path, struct inode *ip);

// fs/blkrw.c
int blk_read(int dev, int blk, int addr, void *buf, size_t size);
int blk_write(int dev, int blk, int addr, const void *buf, size_t size);

// fs/mount.c
int mount_super(struct super_block *s, struct inode *ip);
int umount_super(struct super_block *s);
int umount_inode(struct super_block *ip);
