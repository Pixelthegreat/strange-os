#ifndef FILEIO_H
#define FILEIO_H

#include "../types.h"
#include "fs/fs.h"

typedef int kfd_t;

#define FD_ERROR -1
#define FD_STDIN 0
#define FD_STDOUT 1
#define FD_STDERR 2

/* functions */
extern void kfdinit(void); /* initialize */
extern kfd_t kopen(const char *p, const char *m); /* open file */
extern struct fs_node *kgetnode(kfd_t fd); /* get node for fd */
extern int kread(kfd_t fd, void *buf, size_t n); /* read from file */
extern int kwrite(kfd_t fd, void *buf, size_t n); /* write to file */
extern void kclose(kfd_t fd); /* close file */

#endif /* FILEIO_H */
