#include "../types.h"
#include "../include/string.h"
#include "../include/stdarg.h"
#include "../include/_syscall.h"
#include "../include/starch.h"
#include "../include/stdio.h"

/* print integer */
static void _print_int(int fd, int i) {

	if (!i) {
		write(fd, "0", 1);
		return;
	}
	int sign = 0;
	if (i < 0) {
		i = -i;
		sign = 1;
	}

	char buf[32]; /* buffer */
	size_t n = 0; /* end */
	while (i > 0) {
		buf[n++] = '0' + i % 10;
		i /= 10;
	}

	/* print */
	if (sign) write(fd, "-", 1);
	char c;
	while (n) {
		c = buf[--n];
		write(fd, &c, 1);
	}
}

/* print hex */
const char *hex = "0123456789abcdef";
static void _print_hex(int fd, u32 x) {

	if (!x) {
		write(fd, "0", 1);
		return;
	}

	char buf[32]; /* buffer */
	size_t n = 0; /* end */
	while (x > 0) {
		buf[n++] = hex[x & 0xf];
		x /= 16;
	}

	char c;
	while (n) {
		c = buf[--n];
		write(fd, &c, 1);
	}
}

/* print formatted */
extern void _std_fdprintf(int fd, const char *fmt, ...) {

	va_list args;
	va_start(args, fmt);

	char c;
	while ((c = *fmt++) != 0) {

		/* format specifier */
		if (c == '%') {
			c = *fmt++;

			/* integer */
			if (c == 'd' || c == 'i') {
				int i = va_arg(args, int);
				_print_int(fd, i);
			}

			/* hex value */
			else if (c == 'x') {
				u32 x = va_arg(args, u32);
				_print_hex(fd, x);
			}
			
			/* character */
			else if (c == 'c') {
				char c = va_arg(args, char);
				write(fd, &c, 1);
			}

			/* string */
			else if (c == 's') {
				const char *s = va_arg(args, const char *);
				write(fd, (void *)s, strlen(s));
			}
		}

		/* otherwise */
		else write(fd, &c, 1);
	}
}
