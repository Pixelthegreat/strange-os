/* vga16 vga driver */
#include "../../types.h"
#include "../../include/string.h"
#include "../util.h"
#include "../heap.h"
#include "vpal.h"
#include "vga16.h"
#include "font.h"

static u8 *const vga_intern_addr = (u8 *)0xA0000;

/* values */
int vga_width = 0;
int vga_height = 0;
int vga_bpp = 0;
u8 *vga_addr = NULL;
u8 vga_font_color = 0x7;

static int vga_enabled = 0;

/* 320x200 256 color mode (vga16 mode A) */
static u8 mode_a[] = {
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
			vga_addr[vga_width * y + x] = 0x00;
}

/* initialize vga */
extern void vga_init(int w, int h, int bpp) {

	vga_width = w;
	vga_height = h;
	vga_bpp = bpp;
	vga_addr = kmalloc(vga_width * vga_height);
	
	/* enable mode 13 */
	vga_write_regs(mode_a);

	/* write color palette info */
	for (int i = 0; i < 256; i++) {

		portbout(VGA_DAC_WRITE_IDX, i);
		portbout(VGA_DAC_DATA, vga_palette[i * 3] & 0x3f);
		portbout(VGA_DAC_DATA, vga_palette[i * 3 + 1] & 0x3f);
		portbout(VGA_DAC_DATA, vga_palette[i * 3 + 2] & 0x3f);
	}
	
	/* clear screen */
	vga_cls();
}

/* set pixel */
extern void vga_set_pixel(int x, int y, u8 c) {

	if (x < 0 || x >= vga_width || y < 0 || y >= vga_height) return;
	vga_addr[(y * vga_width) + x] = c;
}

/* draw character */
static void _vga_draw_char(int px, int py, int c, int f) {

	if (c < 0 || c >= 256) return;

	/* get array ptr */
	u8 *g = &vga_font[c * 8];
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if (g[y] & (1 << x)) vga_set_pixel(px + x, py + y, vga_font_color);
			else if (f) vga_set_pixel(px + x, py + y, 0);
		}
	}
}

/* draw character */
extern void vga_draw_char(int px, int py, int c) {

	_vga_draw_char(px, py, c, 0);
}

/* draw character with background */
extern void vga_draw_char_fill(int px, int py, int c) {

	_vga_draw_char(px, py, c, 1);
}

/* update screen */
extern void vga_update(void) {

	memcpy(vga_intern_addr, vga_addr, vga_width * vga_height);
}
