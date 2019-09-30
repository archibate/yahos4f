#pragma once

#include <sys/types.h>

#define READ 0
#define WRITE 1

#define ROOT_INO 2

#define ROOT_DEV root_dev
#define HDA_DEV 1
#define HDB_DEV 2

#define TTY_DRV 1
#define ZERO_DRV 2
#define NULL_DRV 3
#define NR_DRV 16

#define SUPER_MAGIC 0xef53

#define NR_OPEN 20
#define NR_INODE 32
#define NR_FILE 64
#define NR_SUPER 8
#define NR_BUFFERS 128
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

#if 1
#include <sys/stat.h>
#else
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

#define S_DFLNK	(S_IFLNK | 0777)
#define S_DFREG	(S_IFREG | 0644)
#define S_DFDIR	(S_IFDIR | 0755)

#define S_ISFIFO(m)	((m & S_IFMT) == S_IFIFO)
#define S_ISDIR(m)	((m & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)	((m & S_IFMT) == S_IFCHR)
#define S_ISBLK(m)	((m & S_IFMT) == S_IFBLK)
#define S_ISREG(m)	((m & S_IFMT) == S_IFREG)
#define S_ISLNK(m)	((m & S_IFMT) == S_IFLNK)
#define S_ISSOCK(m)	((m & S_IFMT) == S_IFSOCK)
#endif

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
	struct super_block *i_sb;
	unsigned int i_dev;
	unsigned int i_ino;
	unsigned int i_count;
	unsigned int i_on_block;
	unsigned int i_on_addr;
};

struct file {
	unsigned int f_mode;
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

struct char_drive {
	size_t (*read)(struct inode *ip, off_t pos, void *buf, size_t size);
	size_t (*write)(struct inode *ip, off_t pos, const void *buf,
			size_t size);
};

extern struct char_drive drv_table[NR_DRV];
extern struct super_block super_block[NR_SUPER];
extern struct buf *start_buffer;
extern int nr_buffers;
extern int root_dev;

// fs/rwblk.c
void ll_rw_block(int rw, struct buf *b);

// fs/buffer.c
void init_buffer(void);
struct buf *bread(int dev, int block);
void bwrite(struct buf *b);
void brelse(struct buf *b);
int sys_sync(void);

// fs/super.c
struct super_block *get_super(int dev);
struct super_block *read_super(int dev);
void update_super(struct super_block *sb);
void put_super(int dev);

// fs/inode.c
struct inode *iget(int dev, int ino);
struct inode *idup(struct inode *ip);
size_t iread(struct inode *ip, off_t pos, void *buf, size_t size);
size_t iwrite(struct inode *ip, off_t pos, const void *buf, size_t size);
void iupdate(struct inode *ip);
void iput(struct inode *ip);
void sync_inodes(void);
void ext2_free_inode(struct inode *ip);
struct inode *ext2_alloc_inode(struct inode *ip);
void init_inode(struct inode *ip, unsigned int mode, unsigned int nod);

// fs/dir.c
struct inode *dir_creat(struct inode *dip, const char *name, unsigned int mode,
		unsigned int nod);
int dir_find(struct inode *dip, struct dir_entry *de, const char *na, off_t pos);
int dir_link(struct inode *dip, const char *name, struct inode *ip);
int dir_unlink(struct inode *dip, const char *name, int rd);
int dir_init(struct inode *dip, struct inode *pip);
int dir_destroy(struct inode *dip);

// fs/path.c
struct inode *dir_geti(struct inode *dip, const char *path);
struct inode *dir_getp(struct inode *dip, const char **ppath);
struct inode *dir_creati(struct inode *dip, const char *path, unsigned int mode,
		unsigned int nod);
int dir_mkdiri(struct inode *dip, const char *path, unsigned int mode);
int dir_linki(struct inode *dip, const char *path, struct inode *ip);
int dir_unlinki(struct inode *dip, const char *path);
int dir_rmdiri(struct inode *dip, const char *path);

// fs/namei.c
struct inode *namei(const char *path);
struct inode *namep(const char **path);
int linki(const char *path, struct inode *ip);
struct inode *creati(const char *path, unsigned int mode);
int fs_mknod(const char *path, unsigned int mode, unsigned int nod);
int fs_link(const char *oldpath, const char *newpath);
int fs_mkdir(const char *path, unsigned int mode);
int fs_unlink(const char *path);
int fs_rmdir(const char *path);

// fs/blkrw.c
int blk_read(int dev, int blk, int addr, void *buf, size_t size);
int blk_write(int dev, int blk, int addr, const void *buf, size_t size);

// fs/exec.c
int do_execve(const char *path, char *const *argv, char *const *envp);

// fs/vfs.c
void init_fs(void);
