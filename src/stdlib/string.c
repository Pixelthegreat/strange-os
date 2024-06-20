#include "../types.h"
#include "../include/string.h"

/* get length of string */
extern ssize_t _std_strlen(const char *s) {

	ssize_t n = 0;
	while (*s++ != 0) n++;
	return n;
}

/* compare strings */
extern size_t strcmp(char *a, char *b) {

	int i = 0;
	while (a[i] == b[i]) {
		if (a[i] == 0 ||  b[i] == 0) break;
		i++;
	}
	if (a[i] == 0 || b[i] == 0) return 0;
	return (size_t)a[i] - (size_t)b[i];
}
