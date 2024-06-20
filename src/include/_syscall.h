#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "../types.h"

extern int _syscall3(u32 id, u32 a, u32 b, u32 c); /* 3 argument syscall */

#endif /* _SYSCALL_H */
