/* ext2 filesystem driver */

#ifndef _EXT2_H
#define _EXT2_H

#include "../../types.h"
#include "fs.h"

/* filesystem types and permissions */
#define EXT2_FIFO 0x1000
#define EXT2_CHARDEV 0x2000
#define EXT2_DIR 0x4000
#define EXT2_BLKDEV 0x6000
#define EXT2_REG 0x8000
#define EXT2_SYMLNK 0xA000
#define EXT2_UNIXSOCK 0xC000

#define EXT2_OTHER_EXEC 0x001
#define EXT2_OTHER_WRITE 0x002
#define EXT2_OTHER_READ 0x004
#define EXT2_GROUP_EXEC 0x008
#define EXT2_GROUP_WRITE 0x010
#define EXT2_GROUP_READ 0x020
#define EXT2_USER_EXEC 0x040
#define EXT2_USER_WRITE 0x080
#define EXT2_USER_READ 0x100
#define EXT2_STICKY 0x200
#define EXT2_SETUID 0x400
#define EXT2_SETGID 0x800

/* ext2 flags */
#define EXT2_SECURE_DEL 0x1
#define EXT2_KEEP_COPY_DEL 0x2
#define EXT2_COMP 0x4
#define EXT2_SYNC_UPD 0x8
#define EXT2_IMUT 0x10
#define EXT2_APPEND 0x20
#define EXT2_DUMP 0x40
#define EXT2_LACCT_NO_UPD 0x80
#define EXT2_HASH_IDX_DIR 0x10000
#define EXT2_AFS_DIR 0x20000
#define EXT2_JFD 0x40000

/* block */
typedef int ext2_block_t;

/* ext2 fs superblock */
struct ext2_sb {
	u32 inodes; /* total number of inodes */
	u32 blocks; /* total number of blocks */
	u32 sblocks; /* superuser blocks */
	u32 ublocks; /* unallocated blocks */
	u32 uinodes; /* unalloc'd inodes */
	u32 sbb; /* superblock block number */
	u32 bs; /* block size */
	u32 fs; /* fragment size */
	u32 nbpg; /* number of blocks per each group */
	u32 nfpg; /* ^ fragments */
	u32 nipg; /* ^ inodes */
	u32 lmnt; /* last mount time */
	u32 lwrt; /* last write time */
	u16 lnfsckmnt;
	u16 nfsckmnt;
	u16 sig; /* ext2 signature */
} __attribute__((aligned(1024)));

/* block group descriptor */
struct ext2_bgd {
	ext2_block_t bub; /* block usage bitmap */
	ext2_block_t iub; /* inode usage bitmap */
	ext2_block_t itb; /* inode table */
	u16 nub; /* number of unallocated blocks */
	u16 nui; /* number of unallocated inodes */
	u16 nd; /* number of directories */
	char pad[4]; /* unused */
};

/* inode */
struct ext2_inode {
	u16 tp; /* type and permissions */
	u16 uid; /* user id */
	u32 sz_low; /* lower 32 bits of size (in bytes) */
	u32 l_acct; /* last access time (posix time) */
	u32 ct; /* creation time (posix time) */
	u32 l_modt; /* last modification time (posix time) */
	u32 dt; /* deletion time (posix time) */
	u16 gid; /* group id */
	u16 hdl; /* count of hard links to this inode */
	u32 ds; /* disk sectors in use by this inode */
	u32 flags; /* flags */
	u32 osp1; /* os specific value 1 */
	int dbp0, dbp1, dbp2, dbp3, dbp4, dbp5, dbp6, dbp7, dbp8, dbp9, dbp10, dbp11; /* first 12 direct block pointers */
	int sibp; /* singly indirect block pointer (points to a block that is a list of block pointers to data) */
	int dibp; /* doubly indirect block pointer (points to a block that is a list of block pointers to singly indirect blocks) */
	int tipb; /* triply indirect block pointer */
	u32 gn; /* generation number */
	u32 resv_0, resv_1; /* reserved */
	u32 bf; /* block address of fragment */
	char osp2[12]; /* os specific value 2 */
};

/* directory entry */
struct ext2_dirent {
	int inode; /* inode number for file */
	u16 esz; /* entry size including subfields */
	u8 nl; /* name length least significant 8 bits */
	u8 ti; /* type indicator or most significant 8 bits of name length */
	char name[1]; /* name characters */
};

extern int ext2_check_sig(int); /* check if there is an ext2 filesystem on a device */

#endif /* _EXT2_H */
