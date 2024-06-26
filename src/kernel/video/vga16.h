/* base vga driver */
#ifndef VGA16_H
#define VGA16_H

/* include headers */
#include "../../types.h"
#include "../kprint.h"

/* ports */
#define VGA_AC_IDX 0x3c0
#define VGA_AC_WRITE 0x3c0
#define VGA_AC_READ 0x3c1
#define VGA_MISC_WRITE 0x3c2
#define VGA_SEQ_IDX 0x3c4
#define VGA_SEQ_DATA 0x3c5
#define VGA_DAC_READ_IDX 0x3c7
#define VGA_DAC_WRITE_IDX 0x3c8
#define VGA_DAC_DATA 0x3c9
#define VGA_MISC_READ 0x3cc
#define VGA_GC_IDX 0x3ce
#define VGA_GC_DATA 0x3cf
#define VGA_CRTC_IDX 0x3d4
#define VGA_CRTC_DATA 0x3d5
#define VGA_INSTAT_READ 0x3da
#define VGA_NUM_SEQ_REGS 5
#define VGA_NUM_CRTC_REGS 25
#define VGA_NUM_GC_REGS 9
#define VGA_NUM_AC_REGS 21
#define VGA_NUM_REGS (1+(VGA_NUM_SEQ_REGS+VGA_NUM_CRTC_REGS+VGA_NUM_GC_REGS+VGA_NUM_AC_REGS))

/* values */
extern int vga_width;
extern int vga_height;
extern int vga_bpp;
extern u8 *vga_addr;
extern u8 vga_font_color;

/* functions */
extern void vga_write_regs(unsigned char *regs);
extern void vga_cls(void);
extern void vga_init(int w, int h, int bpp);
extern void vga_set_pixel(int x, int y, u8 c);
extern void vga_draw_char(int px, int py, int c);
extern void vga_draw_char_fill(int px, int py, int c);
extern void vga_update(void);

/* driverctl */
#define VIDEO_CMD_GET_WIDTH 0x10
#define VIDEO_CMD_GET_HEIGHT 0x11
#define VIDEO_CMD_SET_INDEX 0x12
#define VIDEO_CMD_SET_PIXEL 0x13

extern u32 vga_driverctl(u32 cmd, u32 p);

#endif /* VGA16_H */
