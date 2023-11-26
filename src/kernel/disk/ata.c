#include "ata.h" /* ata stuff */
#include "../../types.h" /* kernel types */
#include "../../include/unistd.h" /* unistd io */
#include "../util.h" /* utilities */
#include "../idt.h" /* idt */
#include "../../include/string.h" /* string */
#include "../kprint.h" /* print stuff */

/* forward decls */
static int ata_wait(int); /* wait for ide to become ready */
static u8 ata_err(void); /* get an ide error if there is any */

/* variables */
static int hd1; /* disk 1 */
static int hd0; /* disk 0 */
static int hdn; /* default disk */
static int _ata_irq_called = 0;

/* temp data */
static u8 _ns; /* sector start */
static u8 _nn; /* n of sectors */
static u8 _nh; /* head */
static u16 _nc; /* cylinder */
static void *_nbuf; /* buffer to read from */
static int _ncmd; /* command sent */
static int _md; /* mode (chs = 0, lba = 1) */

/* wait for ide to become ready */
static int ata_wait(int chk_err) {

	u8 r;
	
	/* wait for disk to be ready */
	while ((r = portbin(0x1f7)) & (ATA_BSY|ATA_DRDY) != ATA_DRDY) {
	}
	
	/* check for errors */
	if (chk_err && (r & (ATA_DF|ATA_ERR)) != 0)
		return -1;
	
	/* no error or didn't check */
	return 0;
}

/* return error if there is one */
static u8 ata_err() {

	/* get alternate status */
	u8 status = portbin(0x3f6);
	
	/* check for errors */
	if (status & ATA_ERR)
		return portbin(0x1f1);
	
	/* otherwise */
	return 0;
}

/* initialise */
extern void ata_init(void) {

	hd0 = 0;
	hd1 = 0;

	/* no disk on ata0 */
	if (portbin(0x1f7) != 0xff) {
	
		hd0 = 1;

		/* found disk 0 */
		char *s = "[ata] found disk 0\n";
		write(1, s, strlen(s));
	}

	int i;
	
	/* wait for ide */
	ata_wait(0);
	
	/* check if disk 1 is present */
	portbout(0x1f6, 0xe0 | (1 << 4));
	
	for (i = 0; i < 1000; i++) {
	
		if (portbin(0x1f7) != 0) {
		
			hd1 = 1;
			break; /* we can use disk 1 */
		}
	}

	/* switch back to disk 0 */
	portbout(0x1f6, 0xe0 | (0 << 4));
	
	/* print if we are using disk 1 */
	char *hd_sm = "[ata] found disk 1\n";
	if (hd1) write(1, hd_sm, strlen(hd_sm));
}

/* start lba data request */
extern void ata_start_lba(int cmd, u32 s, u8 n, void *buf) {

	/* wait for ide */
	ata_wait(0);
	
	int read_cmd = (n == 1)? ATA_CMD_READ: ATA_CMD_RDMUL;
	int write_cmd = (n == 1)? ATA_CMD_WRITE: ATA_CMD_WRMUL;
	
	/* generate interrupt */
	portbout(0x3f6, 0);
	
	/* data */
	portbout(0x1f6, 0xe0 | (hdn << 4) | ((s >> 24) & 0xf));
	portbout(0x1f2, n);
	portbout(0x1f3, (u8)(s & 0xff));
	portbout(0x1f4, (u8)((s >> 8) & 0xff));
	portbout(0x1f5, (u8)((s >> 16) & 0xff));
	
	/* write */
	if (cmd == ATA_CMD_WRITE) {

		portbout(0x1f7, write_cmd);
		outsl(0x1f0, (u32)buf, 128 * n);
	}
	
	/* read */
	else if (cmd == ATA_CMD_READ) {

		/* store temp data */
		_ns = s;
		_nn = n;
		_nbuf = buf;
		_ncmd = cmd;
		_md = 1;
		
		portbout(0x1f7, read_cmd);
		
		/* wait */
		while (!_ata_irq_called) {}
		_ata_irq_called = 0;
	}
}

/* start data request */
extern void ata_start(int cmd, u8 s, u8 n, u8 h, u16 c, void *buf) {

	/* wait for ide */
	ata_wait(0);
	
	/* command info */
	int read_cmd = (n == 1) ? ATA_CMD_READ: ATA_CMD_RDMUL; /* if we have 1 sector, we just need to call read; if we have multiple, then we should call read multiple */
	int write_cmd = (n == 1) ? ATA_CMD_WRITE: ATA_CMD_WRMUL;
	
	/* generate interrupt */
	portbout(0x3f6, 0);
	
	/* data */
	portbout(0x1f6, 0xe0 | (hdn << 4) | (h & 0x0f));
	portbout(0x1f2, n);
	portbout(0x1f3, s);
	portbout(0x1f4, c & 0xff);
	portbout(0x1f5, (c >> 8) & 0xff);
		
	/* write */
	if (cmd == ATA_CMD_WRITE) {
	
		/* send data */
		portbout(0x1f7, write_cmd);
		outsl(0x1f0, (u32)buf, 128 * n);
	}
	
	/* read */
	else {
		
		/* store temp data */
                _ns = s;
                _nn = n;
                _nh = h;
                _nc = c;
                _nbuf = buf;
                _ncmd = cmd;
	
		portbout(0x1f7, read_cmd);
	
		/* wait */
		while (!_ata_irq_called) {}
		_ata_irq_called = 0;
	}
}

/* interrupt handler */
extern void ata_irq(registers *r) {
	
	/* notify ata_start of ata_irq being called */
	_ata_irq_called = 1;

	/* read data if needed */
	if (_ncmd == ATA_CMD_READ) {
	
		/* read data into buffer */
		insl(0x1f0, _nbuf, 128 * _nn);
	}
}

/* return bitwise info for what disks are available (if there are any) */
extern int ata_get_dev(void) {

	return (hd1 << 1) | hd0;
}

extern void ata_dev(int n) {
	
	hdn = n;
}
