/* header for file buffers and things */
#ifndef _FS_H
#define _FS_H

#include "../../types.h"
#include "../dev.h"

/* macros */
#define FS_FILE 0x01
#define FS_DIR  0x02
#define FS_CDEV 0x03
#define FS_BDEV 0x04
#define FS_PIPE 0x05
#define FS_SLNK 0x06
#define FS_MNTP 0x08

struct fs_info;

/* directory entry */
struct dirent {
	char name[128];
	u32 inode;
	struct fs_node *node; /* file system node (if there is one) */
	struct dirent *next; /* next directory entry */
};

/* file node */
struct fs_node {
	char name[128]; /* file name */
	u32 mask; /* permissions mask */
	u32 flags; /* file flags */
	u32 uid; /* owning user */
	u32 gid; /* owning group */
	u32 inode; /* number of node in filesystem (applies to some but not all) */
	u32 len; /* length/size of file in bytes */
	u32 impl;
	kdev_t dev; /* device that the node is on */
	int part; /* partition */
	struct fs_info *fs; /* filesystem info */
	struct fs_node *ptr; /* for symlinks and mountpoints */
	struct dirent *first; /* first entry for directory */
};

/* function types */
typedef u32 (*fs_read_t)(struct fs_node *, u32, u32, u8 *);
typedef u32 (*fs_write_t)(struct fs_node *, u32, u32, u8 *);
typedef void (*fs_open_t)(struct fs_node *);
typedef void (*fs_close_t)(struct fs_node *);
typedef struct dirent * (*fs_readdir_t)(struct fs_node *, u32);
typedef struct fs_node * (*fs_finddir_t)(struct fs_node *, char *);

extern struct fs_node *fs_root; /* root of the filesystem */

/* filesystem ids */
#define FS_VFS 0 /* virtual filesystem */
#define FS_EXT2 1 /* ext2 filesystem */

/* filesystem info */
struct fs_info {
	u32 id; /* OS filesystem identifier */
	fs_read_t r; /* read */
	fs_write_t w; /* write */
	fs_open_t o; /* open */
	fs_close_t c; /* close */
	fs_readdir_t rd; /* readdir */
	fs_finddir_t fd; /* finddir */
};

/* functions */
extern int fs_init(void); /* intialize */
extern u32 fs_read(struct fs_node *, u32, u32, u8 *); /* read to buffer */
extern u32 fs_write(struct fs_node *, u32, u32, u8 *); /* write to buffer */
extern void fs_open(struct fs_node *); /* open file */
extern void fs_close(struct fs_node *); /* close file */
extern struct dirent *fs_readdir(struct fs_node *, u32); /* read from a directory listing */
extern struct fs_node *fs_finddir(struct fs_node *, char *); /* find a directory */

#endif /* _FS_H */
