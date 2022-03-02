#include "kprint.h" /* print utils */
#include "util.h" /* utilities */

/* variables */
char *vid_mem = (char *)0xb8000; /* start of video memory */
int vid_idx = 0; /* index of characters in video memory */
u8 currsc = 0; /* current scancode */
u8 recsc = 0; /* for kgetsc */

/* forward decls */
static void setcur(int);
static int getcur(void);

/* scancode conversion table */
char scancodes[] = "??1234567890-=??qwertyuiop[]\n?asdfghjkl;\'`?\\zxcvbnm,./??? \?\?!@#$%^&*()_+??QWERTYUIOP{}??ASDFGHJKL:\"~?\\ZXCVBNM<>???? ";

/* hexadecimal characters */
char hex_chars[] = "0123456789ABCDEF";

/* print character */
extern int kprintc(char c) {

	/* if character is not newline then print it normally */
	if (c != '\n')
		vid_mem[(vid_idx++) * 2] = c;
	/* otherwise */
	else kprintnl(); /* print newline */

	/* update scroll pos */
	kscroll();

	/* yes */
	return 0;
}

/* print a string */
extern int kprint(char *s) {

	/* printing a string is just a matter of printing multiple chars */
	while (*s) kprintc(*s++);

	/* yes */
	return 0;
}

/* clear the screen */
extern int kcls(void) {

	/* when clearing, we also need to set each color value in a unit */
	for (int i = 0; i < (VGA_WIDTH * VGA_HEIGHT); i++) {
	
		vid_mem[i * 2] = 0; /* char */
		vid_mem[(i * 2) + 1] = VGA_DEFAULT_COLOR; /* default color */
	}
	
	/* reset values */
	vid_idx = 0;
	setcur(0);

	/* asm */

	/* yes */
	return 0;
}

/* print a newline character */
extern int kprintnl(void) {

	/* printing newline just works by printing a certain number of characters until we reach a new line */
	vid_idx += VGA_WIDTH - (vid_idx % VGA_WIDTH);

	setcur(vid_idx);

	/* yes */
	return 0;
}

/* scrolling */
extern int kscroll(void) {

	/* check if we are at last char in vid mem */
	if (vid_idx >= (VGA_WIDTH * VGA_HEIGHT)) {
	
		/* time to copy */
		for (int i = VGA_WIDTH * 2; i < (((VGA_WIDTH * VGA_HEIGHT) - VGA_WIDTH) * 2); i++)
			vid_mem[i - (VGA_WIDTH * 2)] = vid_mem[i];

		/* remove top row of chars */
		for (int i = 0; i < VGA_WIDTH; i++) {
		
			vid_mem[((VGA_WIDTH * VGA_HEIGHT) - VGA_WIDTH + i) * 2] = 0; /* char */
			vid_mem[((VGA_WIDTH * VGA_HEIGHT) - VGA_WIDTH + i) * 2 + 1] = VGA_DEFAULT_COLOR; /* color */
		}

		/* reset video index */
		vid_idx = (VGA_WIDTH * VGA_HEIGHT) - VGA_WIDTH;
	}
	
	/* set cursor position */
	setcur(vid_idx);

	/* yes */
	return 0;
}

/* get a scancode from stdin */
extern u8 kgetsc(void) {

	/* wait for key press */
	while (!recsc) {}
	recsc = 0; /* reset value */
	return currsc; /* get current scancode */
}

/* get a character from terminal */
extern char kgetc(void) {

	/* return char from scancode conversion set */
	u8 sc;
	if ((sc = kgetsc()) > 58)
		return 0; /* no scancode */
	
	return scancodes[sc]; /* get scancodes */
}

/* keyboard interrupt */
extern void kbint(registers *r) {
		
	/* set value 'recsc' */
	recsc = 1;

	/* get scancode from keyboard buffer */
	currsc = (portbin(0x60));
}

/* print hex number */
extern void kprinthex(unsigned int h) {

	u8 i = 8;
	while (i > 0) {
	
		kprintc(hex_chars[(h >> ((--i) * 4)) & 0xf]);
	}
}

/* get a string of characters */
char _termbuf[2048]; /* buffer for kgets */

extern char *kgets() {

	/* loop through chars */
	u8 sc; /* scancode */
	char c; /* character */
	int nchars = 0; /* number of chars written to buffer */
	int shiftval = 0; /* is shift key being pressed */

	while (((sc = kgetsc()) < 58? (c = scancodes[sc]) != '\n' : (c = 0) == 0)) {
		
		/* shift key */
		if (sc == SCANCODE_SHIFT_DOWN) shiftval = 1;
		else if (sc == SCANCODE_SHIFT_UP) shiftval = 0;

		/* remove a character */
		if (sc == SCANCODE_BACKSPACE) {
		
			if (nchars > 0) {
				
				_termbuf[--nchars] = 0;
				vid_mem[(--vid_idx) * 2] = 0; /* remove character from video memory */
			}
		}
	
		/* add a character */
		else if ((c != 0) && nchars < 2047 && (sc != SCANCODE_SHIFT_DOWN) && (sc != SCANCODE_SHIFT_UP)) {
			
			if (shiftval != 1) {
				
				/* no shift */
				_termbuf[nchars++] = c;
				kprintc(c); /* character */
			} else {
			
				/* shift */
				_termbuf[nchars++] = scancodes[sc + 58];
				kprintc(scancodes[sc + 58]);
			}
		}
	}
	
	/* add null termination character */
	_termbuf[nchars] = 0;

	/* print a new line */
	kprintnl();
	
	/* return buffer */
	return (char *)_termbuf;
}

/* set cursor pos */
static void setcur(int off) {

	portbout(VGA_CTRL_REGISTER, VGA_OFFSET_HIGH);
	portbout(VGA_DATA_REGISTER, (u8)(off >> 8));
	portbout(VGA_CTRL_REGISTER, VGA_OFFSET_LOW);
	portbout(VGA_DATA_REGISTER, (u8)(off & 0xFF));
}

/* get cursor position */
static int getcur(void) {

	portbout(VGA_CTRL_REGISTER, VGA_OFFSET_HIGH);
	int off = portbin(VGA_DATA_REGISTER) << 8;
	portbout(VGA_CTRL_REGISTER, VGA_OFFSET_LOW);
	off |= portbin(VGA_DATA_REGISTER);
	return off;
}
