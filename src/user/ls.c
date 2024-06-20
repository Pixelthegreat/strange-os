#include "unistd.h"
#include "string.h"

extern char *_std_argv;

static char *err_dir = "no such directory\n";

int main() {

	int fd = open(_std_argv, "r");
	if (fd >= 0) {

		u32 idx = 0;
		const char *name;
		while ((name = readdir(fd, idx++)) != NULL) {

			write(1, (void *)name, strlen(name));
			write(1, "\n", 1);
		}
		close(fd);
	}
	else {

		write(1, err_dir, strlen(err_dir));
	}
	return 0;
}
