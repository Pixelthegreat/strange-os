/* ata header */
#ifndef _ATA_H
#define _ATA_H

#include "../../types.h"
#include "../idt.h"

/* macros */
#define SECTOR_SZ 512
#define ATA_BSY 0x80
#define ATA_DRDY 0x40
#define ATA_DF 0x20
#define ATA_ERR 0x01

#define ATA_CMD_READ 0x20
#define ATA_CMD_WRITE 0x30
#define ATA_CMD_RDMUL 0xC4
#define ATA_CMD_WRMUL 0xC5

/* functions */
extern void ata_init(void); /* initialise */
extern void ata_irq(registers *); /* ata irq */
extern void ata_start(int, u8, u8, u8, u16, void *); /* send ata command */
extern void ata_start_lba(int, u32, u8, void *); /* send ata lba command */
extern int  ata_get_dev(void); /* 0x0: no devices found on ata port, 0x1: device found on primary one, 0x2: device found on secondary one, 0x3: both primary one and secondary one */
extern void ata_dev(int); /* set default device for read and write functions */

/* read from IDE device in ATA PIO mode */
static inline void ata_chs_read(u16 z, u8 a, u8 b, u8 c, void *d) {

	/* just calls other function lol */
	ata_start(ATA_CMD_READ, a, b, c, z, d);
}

/* write to IDE device in ATA PIO mode */
static inline void ata_chs_write(u16 z, u8 a, u8 b, u8 c, void *d) {

	ata_start(ATA_CMD_WRITE, a, b, c, z, d);
}

/* read from IDE device with LBA */
static inline void ata_lba_read(u32 s, u8 n, void *b) {

	ata_start_lba(ATA_CMD_READ, s, n, b);
}

/* write to IDE device with LBA */
static inline void ata_lba_write(u32 s, u8 n, void *b) {

	ata_start_lba(ATA_CMD_WRITE, s, n, b);
}

#endif /* _ATA_H */
