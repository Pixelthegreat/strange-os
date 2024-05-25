#include "mbr.h"
#include "ata.h"
#include "../../include/string.h"
#include "../kprint.h"

int mbr_loaded = 0;
u8 mbr_buf[512];

/* get partition info */
extern struct mbr_part_entry mbr_get_part_entry(int n) {

	/* read mbr/boot sector */
	if (!mbr_loaded) ata_chs_read(0, 0, 1, 0, mbr_buf);
	mbr_loaded = 1;
	
	/* calculate partition table location */
	int ptl = 0x1be + ((n - 1) * 0x10);

	/* load values */
	struct mbr_part_entry mpe;
	mpe.attrs          =          mbr_buf[ptl + 0];
	mpe.chs_addr[0]    =          mbr_buf[ptl + 1];
	mpe.chs_addr[1]    =          mbr_buf[ptl + 2];
	mpe.chs_addr[2]    =          mbr_buf[ptl + 3];
	mpe.p_type         =          mbr_buf[ptl + 4];
	mpe.chs_ls_addr[0] =          mbr_buf[ptl + 5];
	mpe.chs_ls_addr[1] =          mbr_buf[ptl + 6];
	mpe.chs_ls_addr[2] =          mbr_buf[ptl + 7];
	mpe.lba_addr       = *(u32 *)&mbr_buf[ptl + 8];
	mpe.nsects         = *(u32 *)&mbr_buf[ptl + 12];
	
	/* return struct */
	return mpe;
}

/* get part info from a bootsector buffer */
extern struct mbr_part_entry mbr_get_part_entry_buf(void *buf, int n) {

	/* load data */
	struct mbr_part_entry mpe;
	int ptl = 0x1be + ((n - 1) * 0x10);
	memcpy((void *)&mpe, &(((u8 *)buf)[ptl]), sizeof(struct mbr_part_entry));
	
	return mpe;
}
