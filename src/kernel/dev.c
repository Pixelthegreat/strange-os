/* device thing */
#include "dev.h"
#include "disk/ata.h"
#include "panic.h"
#include "util.h"
#include "disk/mbr.h"

/* find root device and partition */
extern struct dev_part dev_init_root(void) {

	int dev;

	/* right now we're just checking ata devices */
	int ata = ata_get_dev();
	
	/* no ata devices */
	if (!ata)
		kpanic(E_NODEV, "no storage devices found\n");
	
	/* hd0 */
	if (ata & 0x1)
		dev = DEV_ATA_HD0;
	/* hd1 */
	else if (ata & 0x2)
		dev = DEV_ATA_HD1;
	
	/* return struct */
	struct dev_part p;
	p.dev = dev;
	
	/* check for bootable flag */
	u8 buf[512];
	
	dev_read_lba(dev, 0, 1, buf);
	
	if (*(u16 *)&buf[510] != 0xaa55 && *(u16 *)&buf[510] != 0x55aa)
		kpanic(E_NODEV, "no bootable device found\n");
	
	/* search partitions */
	int i;
	for (i = 1; i <= 4; i++) {
	
		p.pe = mbr_get_part_entry_buf((void *)&buf, i);
		
		/* right partition type */
		if (p.pe.p_type == MBR_TYPE_LINUX && p.pe.attrs & MBR_BOOTABLE)
			break;
	}
	
	/* could not find a partition */
	if (i > 4)
		kpanic(E_NODEV, "no bootable partition found\n");
	
	/* return part table stuff */
	return p;
}

/* read lba mode */
extern void dev_read_lba(kdev_t dev, int lba, int n, void *buf) {

	/* ata */
	if (dev == DEV_ATA_HD0 || dev == DEV_ATA_HD1) {
	
		/* read ata */
		ata_dev(dev);
		ata_lba_read(lba, n, buf);
		return;
	}
	
	/* panic */
	kpanic(E_NODEV, "unsupported device for reading\n");
}
