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

#define FS_TYPE 0xf
#define FS_NODE_TYPE(n) ((n)->flags & FS_TYPE)

#define FS_MODE_R 0x01
#define FS_MODE_W 0x02

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
	u32 impl; /* directory was read */
	kdev_t dev; /* device that the node is on */
	int part; /* partition */
	struct fs_info *fs; /* filesystem info */
	struct fs_node *ptr; /* for symlinks and mountpoints */
	struct dirent *first; /* first entry for directory */
	struct fs_node *parent; /* parent node */
	u32 impl2[2]; /* more implementation data */
};

/* file */
struct fs_file {
	struct fs_node *n; /* node */
	u32 m; /* mode (r/w) */
	u32 idx; /* index in file */
	void *d; /* extra fs data */
};

/* function types */
typedef u32 (*fs_read_t)(struct fs_file *, u32, void *);
typedef u32 (*fs_write_t)(struct fs_file *, u32, void *);
typedef void (*fs_open_t)(struct fs_file *);
typedef void (*fs_close_t)(struct fs_file *);
typedef void (*fs_filldir_t)(struct fs_node *);
typedef void (*fs_create_t)(struct fs_node *, char *, u32, struct dirent *);

extern struct fs_node *fs_root; /* root of the filesystem */

/* filesystem ids */
#define FS_VFS 0 /* virtual filesystem */
#define FS_EXT2 1 /* ext2 filesystem */
#define FS_FAT32 2 /* fat32 filesystem */

/* filesystem info */
struct fs_info {
	u32 id; /* OS filesystem identifier */
	fs_read_t r; /* read */
	fs_write_t w; /* write */
	fs_open_t o; /* open */
	fs_close_t c; /* close */
	fs_filldir_t fd; /* filldir */
	fs_create_t cr; /* create */
};

/* functions */
extern int fs_init(void); /* intialize */
extern struct fs_node *fs_alloc(struct fs_node *p); /* allocate node */
extern struct dirent *fs_dirent_alloc(void); /* allocate dirent */
extern u32 fs_read(struct fs_file *fp, u32, void *); /* read to buffer */
extern u32 fs_write(struct fs_file *fp, u32, void *); /* write to buffer */
extern struct fs_file *fs_open(struct fs_node *, u32); /* open file */
extern void fs_close(struct fs_file *); /* close file */
extern struct dirent *fs_readdir(struct fs_node *, u32); /* read from a directory listing */
extern struct fs_node *fs_finddir(struct fs_node *, char *); /* find a directory */
extern void fs_filldir(struct fs_node *); /* fill a directory */
extern void fs_create(struct fs_node *, char *, u32); /* create a file */
extern struct fs_node *fs_finddir_path(struct fs_node *, char *); /* find a file with path */

#endif /* _FS_H */
