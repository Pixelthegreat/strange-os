/* device numbers for inside use from the kernel */
#ifndef _DEV_H
#define _DEV_H

/* includes */
#include "../types.h"
#include "disk/mbr.h"

typedef int kdev_t; /* for device numbers */

/* partition */
struct dev_part {
	kdev_t                dev; /* device */
	int                   p; /* partition number */
	struct mbr_part_entry pe; /* partition info */
};

#define DEV_ATA_HD0  0  /* ata first drive */
#define DEV_ATA_HD1  1  /* ata second drive */
#define DEV_CON      2  /* console */
#define DEV_NUL      3  /* nothing */
#define DEV_USB_A0   4  /* first assigned usb port */
#define DEV_USB_A1   5  /* second assigned usb port */
#define DEV_USB_A2   6  /* third assigned usb port */
#define DEV_USB_A3   7  /* fourth assigned usb port */
#define DEV_AHCI_HD0 8  /* ahci first drive */
#define DEV_AHCI_HD1 9  /* ahci second drive */
#define DEV_AHCI_HD2 10 /* ahci third drive */
#define DEV_AHCI_HD3 11 /* anci fourth drive */
#define DEV_KEYBOARD 12 /* keyboard */

/* functions */
extern struct dev_part dev_init_root(void); /* search for the root device and partition */
extern void dev_read_lba(kdev_t, int, int, void *); /* read from a device using lba sector addressing */
extern void dev_write_lba(kdev_t, int, int, void *); /* write to a device using lba sector addressing */

/*
 * procedure/requirements for searching for the root device:
 * - check ata devices
 * - check other (unsupported at the moment) devices
 * - if there is none to check, panic
 * - device must have boot signature
 * - partition table must be mbr
 * - partition must have type linux (0x83) and be bootable
 * - partition must contain the strange-os kernel
 */

#endif /* _DEV_H */
