#include "_syscall.h"
#include "unistd.h"
#include "string.h"
#include "stdarg.h"

extern char *_std_argv;

int main() {

	/* print arguments */
	write(1, _std_argv, strlen(_std_argv));
	write(1, "\n", 1);

	return 0;
}
