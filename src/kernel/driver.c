#include "../types.h"
#include "../include/string.h"
#include "video/vga16.h"
#include "kprint.h"
#include "driver.h"

#define DRIVER_MAX 32
static struct driver_info drivers[DRIVER_MAX];
static int n_drivers = 0;

/* initialize */
extern void driver_init(void) {

	driver_register("KBD", kbctl);
	driver_register("VIDEO", vga_driverctl);
}

/* register driver */
extern void driver_register(const char *name, driver_ctl_t ctl) {

	if (n_drivers >= DRIVER_MAX) return;
	drivers[n_drivers].name = name;
	drivers[n_drivers].ctl = ctl;
	n_drivers++;
}

/* get id */
extern driver_t driver_get_id(const char *name) {

	driver_t id;
	for (id = 0; id < n_drivers; id++) {
		if (kstreq((char *)drivers[id].name, (char *)name))
			return id;
	}
	return -1;
}

/* send command to driver */
extern u32 driver_control(driver_t d, u32 cmd, u32 p) {

	if (d < 0 || d >= n_drivers || drivers[d].ctl == NULL) return 0;	
	return drivers[d].ctl(cmd, p);
}
