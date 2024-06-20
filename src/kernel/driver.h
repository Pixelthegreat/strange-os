/* strange os driver syscalls (driverid, driverctl) */
#ifndef DRIVER_H
#define DRIVER_H

#include "../types.h"

typedef int driver_t; /* driver id */
typedef u32 (*driver_ctl_t)(u32, u32); /* control function */

struct driver_info {
	const char *name; /* name of driver */
	driver_ctl_t ctl; /* control function */
};

/* functions */
extern void driver_init(void); /* initialize */
extern void driver_register(const char *name, driver_ctl_t ctl); /* register driver */
extern driver_t driver_get_id(const char *name); /* get id */
extern u32 driver_control(driver_t d, u32 cmd, u32 p); /* send command to driver */

#endif /* DRIVER_H */
