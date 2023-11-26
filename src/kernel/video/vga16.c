/* vga16 vga driver */
#include "vga16.h"
#include "../util.h"

/* values */
int vga_width = 0;
int vga_height = 0;
int vga_bpp = 0;
u8 *vga_addr = NULL;

static int vga_enabled = 0;

/* 320x200 256 color mode (vga16 mode A) */
static unsigned char mode_a[] = {
	/* misc */
	0x63,
	/* seq */
	0x03, 0x01, 0x0F, 0x00, 0x0E,
	/* crtc */
	0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
	0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x9C, 0x0E, 0x8F, 0x28,	0x40, 0x96, 0xB9, 0xA3,
	0xFF,
	/* gc */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
	0xFF,
	/* ac */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x41, 0x00, 0x0F, 0x00,	0x00
};

/* write registers */
extern void vga_write_regs(unsigned char *regs) {

	/* write misc reg */
	portbout(VGA_MISC_WRITE, *regs);
	regs++;
	
	/* write sequencer regs */
	for (int i = 0; i < VGA_NUM_SEQ_REGS; i++) {

		portbout(VGA_SEQ_IDX, i);
		portbout(VGA_SEQ_DATA, *regs);
		regs++;
	}
	
	/* unlock crtc registers */
	portbout(VGA_CRTC_IDX, 0x03);
	portbout(VGA_CRTC_DATA, portbin(VGA_CRTC_DATA) | 0x80);
	portbout(VGA_CRTC_IDX, 0x11);
	portbout(VGA_CRTC_DATA, portbin(VGA_CRTC_DATA) & ~0x80);
	
	/* make sure they remain unlocked */
	regs[0x03] |= 0x80;
	regs[0x11] &= ~0x80;
	
	/* write crtc regs */
	for (int i = 0; i < VGA_NUM_CRTC_REGS; i++) {

		portbout(VGA_CRTC_IDX, i);
		portbout(VGA_CRTC_DATA, *regs);
		regs++;
	}
	
	/* write graphics controller registers */
	for (int i = 0; i < VGA_NUM_GC_REGS; i++) {

		portbout(VGA_GC_IDX, i);
		portbout(VGA_GC_DATA, *regs);
		regs++;
	}
	
	/* write attribute controller regs */
	for (int i = 0; i < VGA_NUM_AC_REGS; i++) {

		portbout(VGA_AC_IDX, i);
		portbout(VGA_AC_WRITE, *regs);
		regs++;
	}
	
	/* lock 16-color palette and unblank display */
	portbin(VGA_INSTAT_READ);
	portbout(VGA_AC_IDX, 0x20);
}

/* clear vga screen */
extern void vga_cls(void) {

	unsigned int x, y = 0;
	
	/* set colors */
	for (y = 0; y < vga_height; y++)
		for (x = 0; x < vga_width; x++)
			vga_addr[vga_width * y + x] = 0x0f;
}

/* initialize vga */
extern void vga_init(int w, int h, int bpp) {

	vga_width = w;
	vga_height = h;
	vga_bpp = bpp;
	vga_addr = (unsigned char *)0xA0000;
	
	/* enable mode 13 */
	vga_write_regs(mode_a);
	
	/* clear screen */
	vga_cls();
}
