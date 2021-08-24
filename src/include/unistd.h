#ifndef _UNISTD_H
#define _UNISTD_H

#include "../types.h"

/* if we haven't included stdio.h, we should define this stuff... i guess */
#ifndef _STDIO_H

/* default file descriptors */
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

/* seek */
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

/* buffer size */
#define BUFSIZ 2048

#endif /* _STDIO_H */

/* close file descriptor */
extern int close(int);

/* read n bytes to a buffer */
extern ssize_t read(int, void *, size_t);

/* write n bytes from a buffer to a file */
extern ssize_t write(int, void *, size_t);

/* schedule an alarm in S seconds */
extern unsigned int alarm(unsigned int);

/* sleep S seconds */
extern unsigned int sleep(unsigned int);

/* flush a buffer */
extern int flush(int);

// more to do i guess //

#endif /* _UNISTD_H */
