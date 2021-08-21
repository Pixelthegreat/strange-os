#include "kprint.h" /* print utils */

/* variables */
char *vid_mem = (char *)0xb8000; /* start of video memory */
int vid_idx = 0; /* index of characters in video memory */

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
extern int kcls() {

	/* when clearing, we also need to set each color value in a unit */
	for (int i = 0; i < (VGA_WIDTH * VGA_HEIGHT); i++) {
	
		vid_mem[i * 2] = 0; /* char */
		vid_mem[(i * 2) + 1] = VGA_DEFAULT_COLOR; /* default color */
	}
	
	/* reset values */
	vid_idx = 0;

	/* yes */
	return 0;
}

/* print a newline character */
extern int kprintnl() {

	/* printing newline just works by printing a certain number of characters until we reach a new line */
	vid_idx += VGA_WIDTH - (vid_idx % VGA_WIDTH);

	/* yes */
	return 0;
}

/* scrolling */
extern int kscroll() {

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

	/* yes */
	return 0;
}
