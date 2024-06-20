#ifndef _UNISTD_H
#define _UNISTD_H

#include "../types.h"

/* syscall numbers */
#define _SYS_OPEN 1
#define _SYS_READ 2
#define _SYS_WRITE 3
#define _SYS_CLOSE 4
#define _SYS_EXEC 5
#define _SYS_EXIT 6
#define _SYS_READDIR 7

/* functions */
extern int _std_open(const char *p, const char *m); /* open with mode string */
extern size_t _std_read(int fd, void *buf, size_t n); /* read n bytes */
extern size_t _std_write(int fd, void *buf, size_t n); /* write n bytes */
extern void _std_close(int fd); /* close file */
extern int _std_exec(const char *p, char *argv, char **envp); /* execute program */
extern const char *_std_readdir(int fd, u32 idx); /* read directory entry */

#define open _std_open
#define read _std_read
#define write _std_write
#define close _std_close
#define exec _std_exec
#define readdir _std_readdir

#endif /* _UNISTD_H */
