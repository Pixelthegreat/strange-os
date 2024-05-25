#include "../types.h"
#include "util.h"
#include "../include/string.h"
#include "heap.h"
#include "video/vga16.h"
#include "kprint.h"

static char *textbuf = NULL;
static u32 idx = 0; /* index into text memory */
static u8 recsc = 0, cursc = 0; /* scancode info */

/* scancode conversion table */
static char scancodes[] = "??1234567890-=??qwertyuiop[]\n?asdfghjkl;\'`?\\zxcvbnm,./??? \?\?!@#$%^&*()_+??QWERTYUIOP{}??ASDFGHJKL:\"~?\\ZXCVBNM<>???? ";

/* hexadecimal values */
static char hex_chars[] = "0123456789ABCDEF";

/* initialize */
extern void ktextinit(void) {

	if (textbuf == NULL) textbuf = (char *)kmalloc(KPRINT_WIDTH * KPRINT_HEIGHT);
	kcls();
}

/* clear screen */
extern void kcls(void) {

	idx = 0;
	for (int i = 0; i < KPRINT_WIDTH * KPRINT_HEIGHT; i++)
		textbuf[i] = 0;
	kflush();
}

/* print string */
extern void kprint(const char *s) {

	char c;
	while ((c = *s++) != 0)
		kprintc(c);
}

/* print character */
extern void kprintc(char c) {

	if (c == '\n') idx += KPRINT_WIDTH - (idx % KPRINT_WIDTH);
	else textbuf[idx++] = c;

	/* scroll */
	kscroll();
	
	/* flush if newline */
	if (c == '\n') kflush();
}

/* print hexadecimal number */
extern void kprinthex(u32 h) {

	u8 i = 8;
	while (i > 0) {

		kprintc(hex_chars[(h >> ((--i) * 4)) & 0xf]);
	}
}

/* print newline */
extern void kprintnl(void) {

	kprintc('\n');
}

/* scroll */
extern void kscroll(void) {

	if (idx < KPRINT_WIDTH * KPRINT_HEIGHT) return;

	/* copy */
	int lr = KPRINT_WIDTH * (KPRINT_HEIGHT-1); /* last row */
	for (u32 i = 0; i < lr; i++)
		textbuf[i] = textbuf[i + KPRINT_WIDTH];

	/* clear bottom row */
	memset(&textbuf[lr], 0, KPRINT_WIDTH);
	idx = lr;
}

/* flush */
extern void kflush(void) {

	for (int i = 0; i < KPRINT_WIDTH * KPRINT_HEIGHT; i++) {
		int x = (i % KPRINT_WIDTH) * 8;
		int y = (i / KPRINT_WIDTH) * 8;
		vga_draw_char_fill(x, y, textbuf[i]);
	}

	/* draw cursor */
	int cx = (idx % KPRINT_WIDTH) * 8;
	int cy = (idx / KPRINT_WIDTH) * 8;
	for (int y = 0; y < 2; y++) {
		for (int x = 0; x < 8; x++) {
			vga_set_pixel(cx + x, cy + y + 6, vga_font_color);
		}
	}

	/* update */
	vga_update();
}

/* keyboard interrupt */
extern void kbint(registers *regs) {

	recsc = 1;
	cursc = (portbin(0x60));
}

/* get scancode */
extern u8 kgetsc(void) {

	while (!recsc) {}
	recsc = 0;
	return cursc;
}

/* get character */
extern char kgetc(void) {

	u8 sc;
	if ((sc = kgetsc()) > 58) return 0;
	return scancodes[sc];
}

/* get string */
extern void kgets(char *buf, size_t sz) {

	u8 sc = 0; /* scancode */
	char c = 0; /* character */
	int sh = 0; /* shift */
	int nch = 0; /* number of chars read */

	while (c != '\n') {

		if ((sc = kgetsc()) < 58) {

			if (sh) c = scancodes[sc + 58];
			else c = scancodes[sc];
		}
		else c = 0;

		/* shift */
		if (sc == SCANCODE_SHIFT_DOWN) sh = 1;
		else if (sc == SCANCODE_SHIFT_UP) sh = 0;

		/* remove a character */
		if (sc == SCANCODE_BACKSPACE) {

			if (nch > 0) {

				buf[--nch] = 0;
				textbuf[--idx] = 0;
				kflush();
			}
		}

		/* add a character */
		else if (c != 0 && c != '\n' && sc != SCANCODE_SHIFT_DOWN && sc != SCANCODE_SHIFT_UP) {

			if (nch < sz-1) buf[nch] = c;
			kprintc(c);
			kflush();
			nch++; /* increment anyway */
		}
	}

	buf[nch] = 0; /* add null terminator */
	kprintnl();
}
