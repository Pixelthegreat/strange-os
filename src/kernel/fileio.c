#include "../types.h"
#include "../include/string.h"
#include "kprint.h"
#include "fs/fs.h"
#include "fileio.h"

#define MAX_NUM_FDS 32

static struct fs_file *fds[MAX_NUM_FDS];
static struct fs_node *stdin_n, *stdout_n, *stderr_n;
static struct fs_file *stdin_f, *stdout_f, *stderr_f;

/* read input */
static u32 _stdio_read(struct fs_file *fp, u32 c, void *buf) {

	if (fp->m != FS_MODE_R) return 0;
	kgets((char *)buf, c);
	return c; /* update kgets */
}

/* write output */
static u32 _stdio_write(struct fs_file *fp, u32 c, void *buf) {

	if (fp->m != FS_MODE_W) return 0;
	for (u32 i = 0; i < c; i++) kprintc(((char *)buf)[i]);
	kflush();
	return c;
}

static struct fs_info stdio = {
	.r = _stdio_read,
	.w = _stdio_write,
}; /* stdin stdout stderr */

/* init (todo: create mountpoint for /dev) */
extern void kfdinit(void) {

	memset(fds, 0, sizeof(kfd_t) * MAX_NUM_FDS);

	stdin_n = fs_alloc(NULL);
	strcpy(stdin_n->name, "stdin");
	stdin_n->mask = FS_CDEV;
	stdin_n->dev = DEV_CON;
	stdin_n->fs = &stdio;

	/* copies */
	stdout_n = fs_alloc(stdin_n);
	stdout_n->parent = NULL; /* is set with stdin_n */
	stdout_n->mask = FS_CDEV;

	stderr_n = fs_alloc(stderr_n);
	stderr_n->parent = NULL;
	stderr_n->mask = FS_CDEV;

	/* open files */
	stdin_f = fs_open(stdin_n, FS_MODE_R);
	stdout_f = fs_open(stdout_n, FS_MODE_W);
	stderr_f = fs_open(stderr_n, FS_MODE_W);

	fds[FD_STDIN] = stdin_f;
	fds[FD_STDOUT] = stdout_f;
	fds[FD_STDERR] = stderr_f;
}

/* open file */
extern kfd_t kopen(const char *p, const char *m) {

	/* get file node */
	if (p[0] == '/') p++;
	else return -1; /* todo: current working dir */

	struct fs_node *n = fs_finddir_path(fs_root, (char *)p);
	if (n == NULL) return -1;

	/* parse mode string */
	u32 md = 0;
	char c;
	while ((c = *m++) != 0) {

		if (c == 'r') md |= FS_MODE_R;
		else if (c == 'w') md |= FS_MODE_W;
		else return -1; /* invalid mode */
	}

	/* find free descriptor */
	kfd_t fd = -1;
	for (int i = 0; i < MAX_NUM_FDS; i++) {
		if (fds[i] == NULL) {
			fd = i;
			break;
		}
	}

	/* failed to find free descriptor */
	if (fd < 0) return -1;

	/* open file */
	struct fs_file *fp = fs_open(n, md);
	if (fp == NULL) return -1;

	fds[fd] = fp;
	return fd;
}

/* get vfs node */
extern struct fs_node *kgetnode(kfd_t fd) {

	if (fd < 0 || fd >= MAX_NUM_FDS || !fds[fd]) return NULL;
	return fds[fd]->n;
}

/* read from file */
extern int kread(kfd_t fd, void *buf, size_t n) {

	if (fd < 0 || fd >= MAX_NUM_FDS || !fds[fd]) return -1;
	return (int)fs_read(fds[fd], (u32)n, buf);
}

/* write to file */
extern int kwrite(kfd_t fd, void *buf, size_t n) {

	if (fd < 0 || fd >= MAX_NUM_FDS || !fds[fd]) return -1;
	return (int)fs_write(fds[fd], (u32)n, buf);
}

/* close file */
extern void kclose(kfd_t fd) {

	if (fd < 3 || fd >= MAX_NUM_FDS || !fds[fd]) return; /* 3 to account for stdin, stdout and stderr */
	fs_close(fds[fd]);
	fds[fd] = NULL;
}
