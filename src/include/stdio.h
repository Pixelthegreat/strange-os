#ifndef STDIO_H
#define STDIO_H

/* functions */
extern void _std_fdprintf(int fd, const char *fmt, ...); /* print to file descriptor */

#define printf(fmt, ...) _std_fdprintf(1, fmt, __VA_ARGS__)

#endif /* STDIO_H */
