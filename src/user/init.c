/* init program that loads the shell */
#include "_syscall.h"
#include "starch.h"
#include "string.h"

#define SHELL_BUFSZ 32

static char *err_cfg = "failed to open '/etc/init'\n";
static char *err_bin = "failed to execute shell\n";
static char shell[SHELL_BUFSZ]; /* shell executable */

void main() {

	/* open config */
	int fd = open("/etc/init", "r");
	if (fd < 0) {

		write(1, err_cfg, strlen(err_cfg));
		while (1) {}
	}

	/* close file */
	int len = read(fd, shell, SHELL_BUFSZ);
	close(fd);

	write(1, shell, len);
	write(1, "\n", 1);

	/* execute process */
	int res = 0;
	while (1) {
		char *argv = "arguments";
		res = exec(shell, argv, NULL);
		if (res != 0) break;
	}

	/* failed */
	write(1, err_bin, strlen(err_bin));
	while (1) {}
}
