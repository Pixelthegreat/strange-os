#include "starch.h"
#include "string.h"

extern char *_std_argv; /* temp argv solution */

#define BUFSZ 512
static char buf[BUFSZ]; /* file buffer */

static char *err_file = "no such file\n";

int main() {

	int fd = open(_std_argv, "r");
	if (fd < 0) {

		write(1, err_file, strlen(err_file));
		return 1;
	}

	/* read file */
	size_t cnt = 512;
	while (cnt == BUFSZ) {

		cnt = read(fd, buf, BUFSZ);
		write(1, buf, cnt);
	}

	close(fd);
	return 0;
}
