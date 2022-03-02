/* kernel panic system */
#ifndef _PANIC_H
#define _PANIC_H

/* error types */
#define E_PAGEFLT 0 /* page faults */
#define E_NOMEDIA 1 /* no boot media found */
#define E_NOPAGES 2 /* no more alloc'able pages */
#define E_CPUERR  3 /* just a cpu error */
#define E_NOFS    4 /* failed to initialise a filesystem */
#define E_NODEV   5 /* no storage devices found */

extern void kpanic(int, char *);

#endif /* _PANIC_H */
