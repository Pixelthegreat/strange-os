/* header for file buffers and things */
#ifndef _FS_H
#define _FS_H

#include "../../types.h"

/* macros */
#define FS_FILE 0x01
#define FS_DIR  0x02
#define FS_CDEV 0x03
#define FS_BDEV 0x04
#define FS_PIPE 0x05
#define FS_SLNK 0x06
#define FS_MNTP 0x08

typedef struct {
} fbuf;

/* directory entry */
struct dirent {
	char name[128];
	u32 inode;
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
	u32 (*p_read)(struct fs_node *, u32, u32, u8 *); /* read function */
	u32 (*p_write)(struct fs_node *, u32, u32, u8 *); /* write function */
	void (*p_open)(struct fs_node *); /* open function */
	void (*p_close)(struct fs_node *); /* close function */
	struct dirent * (*p_readdir)(struct fs_node *, u32); /* readdir function */
	struct fs_node * (*p_finddir)(struct fs_node *, char *); /* finddir function */
	struct fs_node *ptr; /* for symlinks and mountpoints */
};

/* function types */
typedef u32 (*fs_read_t)(struct fs_node *, u32, u32, u8 *);
typedef u32 (*fs_write_t)(struct fs_node *, u32, u32, u8 *);
typedef void (*fs_open_t)(struct fs_node *);
typedef void (*fs_close_t)(struct fs_node *);
typedef struct dirent * (*fs_readdir_t)(struct fs_node *, u32);
typedef struct fs_node * (*fs_finddir_t)(struct fs_node *, char *);

extern struct fs_node *fs_root; /* root of the filesystem */

/* functions */
extern int fs_init(void); /* intialize */
extern u32 fs_read(struct fs_node *, u32, u32, u8 *); /* read to buffer */
extern u32 fs_write(struct fs_node *, u32, u32, u8 *); /* write to buffer */
extern void fs_open(struct fs_node *); /* open file */
extern void fs_close(struct fs_node *); /* close file */
extern struct dirent *fs_readdir(struct fs_node *, u32); /* read from a directory listing */
extern struct fs_node *fs_finddir(struct fs_node *, char *); /* find a directory */

#endif /* _FS_H */
