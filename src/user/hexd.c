#include "../types.h"
#include "starch.h"
#include "string.h"
#include "stdio.h"

static char *err_file = "can't open file\n";

#define BUFSZ 512
static u8 buf[BUFSZ]; /* file buffer */

extern char *_std_argv;

static void _print_byte(u8 h) {
	printf("%x%x", (h >> 4) & 0xf, h & 0xf);
}
static void _print_word(u16 h) {
	printf("%x%x%x%x", (h >> 12) & 0xf, (h >> 8) & 0xf, (h >> 4) & 0xf, h & 0xf);
}

int main() {

	u16 addr = 0;

	/* open file */
	int fd = open(_std_argv, "r");
	if (fd < 0) {

		printf("can't open file: '%s'\n", _std_argv);
		return 1;
	}

	/* read */
	size_t nb = BUFSZ;
	while (nb == BUFSZ) {

		nb = read(fd, buf, BUFSZ);
		size_t b = 0;
		while (b < nb) {

			/* print bytes */
			size_t r = nb-b < 8? nb-b: 8;
			_print_word(addr);
			write(1, " | ", 3);
			for (int i = 0; i < r; i++) { _print_byte(buf[b+i]); write(1, " ", 1); }
			write(1, "\n", 1);

			addr += r;
			b += r;
		}
	}

	close(fd);
	return 0;
}
