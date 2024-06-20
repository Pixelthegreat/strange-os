#include "_syscall.h"
#include "unistd.h"
#include "string.h"

static char *err_cmd = "command not found\n";
static char *prompt = "$ ";
static char buf[128]; /* text buffer */
static char cmdbuf[128] = "/bin/"; /* command buffer */
static u32 m = 0;

extern char *_std_argv;

/* compare strings */
static int streq(const char *a, const char *b) {

	while ((*a != 0) && (*b != 0)) {
		if (*a != *b) return 0;
		a++;
		b++;
	}
	return *a == *b;
}

int main() {

	char *cmd = cmdbuf + 5; /* command */

	while (1) {

		write(1, prompt, strlen(prompt));
		size_t nb = read(0, buf, 128);
		buf[nb] = 0;

		if (!buf[0]) continue;

		/* parse beginning */
		int i = -1;
		while (buf[++i] != 0 && buf[i] != ' ')
			cmd[i] = buf[i];
		cmd[i] = 0;

		/* exit */
		if (streq(cmd, "exit")) break;

		/* exec */
		char *argv = buf[i] == ' '? &buf[i+1]: "";
		char *path = (cmd[0] == '/')? cmd: cmdbuf;
		int res = exec(path, argv, NULL);

		if (res < 0) {
			write(1, "sh: ", 4);
			write(1, cmd, i);
			write(1, ": ", 2);
			write(1, err_cmd, strlen(err_cmd));
		}
	}
	return 0;
}
