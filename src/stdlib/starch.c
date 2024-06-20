#include "../types.h"
#include "../include/_syscall.h"
#include "../include/starch.h"

/* open */
extern int _std_open(const char *p, const char *m) {

	return (int)_syscall3(_SYS_OPEN, (u32)p, (u32)m, 0);
}

/* read */
extern size_t _std_read(int fd, void *buf, size_t n) {

	return (size_t)_syscall3(_SYS_READ, (u32)fd, (u32)buf, (u32)n);
}

/* write */
extern size_t _std_write(int fd, void *buf, size_t n) {

	return (size_t)_syscall3(_SYS_WRITE, (u32)fd, (u32)buf, (u32)n);
}

/* close */
extern void _std_close(int fd) {

	return (void)_syscall3(_SYS_CLOSE, (u32)fd, 0, 0);
}

/* exec */
extern int _std_exec(const char *p, char *argv, char **envp) {

	return (int)_syscall3(_SYS_EXEC, (u32)p, (u32)argv, (u32)envp);
}

/* readdir */
extern const char *_std_readdir(int fd, u32 idx) {

	return (const char *)_syscall3(_SYS_READDIR, (u32)fd, idx, 0);
}

/* get driver id */
extern int _std_driverid(const char *name) {

	return (int)_syscall3(_SYS_DRIVERID, (u32)name, 0, 0);
}

/* send command to driver */
extern u32 _std_driverctl(int id, u32 cmd, u32 p) {

	return _syscall3(_SYS_DRIVERCTL, (u32)id, cmd, p);
}
