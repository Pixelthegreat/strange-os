/* graphics test program */
#include "_syscall.h"
#include "starch.h"
#include "string.h"
#include "stdio.h"

static char *err_kbd = "failed to initialize keyboard\n";
static char *err_vid = "failed to initialize video\n";

#define KBD_WAIT_KEY 0x10

#define VID_GET_WIDTH 0x10
#define VID_GET_HEIGHT 0x11
#define VID_SET_INDEX 0x12
#define VID_SET_PIXEL 0x13

int main() {

	/* get keyboard driver id */
	int kbd = driverid("KBD");
	if (kbd < 0) {

		write(1, err_kbd, strlen(err_kbd));
		return 1;
	}

	/* get video driver id */
	int vga = driverid("VIDEO");
	if (vga < 0) {

		write(1, err_vid, strlen(err_vid));
		return 1;
	}

	/* get width and height */
	u32 w = driverctl(vga, VID_GET_WIDTH, 0);
	u32 h = driverctl(vga, VID_GET_HEIGHT, 0);

	/* set pixel */
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			driverctl(vga, VID_SET_INDEX, y * w + x);
			driverctl(vga, VID_SET_PIXEL, 0x0c);
		}
	}
	u32 sc = driverctl(kbd, KBD_WAIT_KEY, 0);

	return 0;
}
