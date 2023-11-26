#include "../../include/string.h" /* stdlib string functions */

/* return length of string in chars */
extern ssize_t strlen(char *s) {

	ssize_t res; /* variable to hold result */
	for (res = 0; *s++; res++) {} /* get string length */
	return res; /* return value */
}

/* compare strings */
extern int strcmp(char *a, char *b) {

	int i = 0;
	while (a[i] == b[i]) {
		if (a[i] == 0 || b[i] == 0) break;
		i++;
	}
	if (a[i] == 0 || b[i] == 0) return 0;
	return (int)a[i] - (int)b[i];
}

/* copy n bytes from a to b */
extern void *memcpy(void *b, void *a, size_t n) {

	/* loop */
	for (size_t i = 0; i < n; i++)
		((char *)b)[i] = ((char *)a)[i];
}

/* strcpy */
extern char *strcpy(char *dst, char *src) {

	ssize_t len = strlen(src);
	memcpy(dst, src, len);
	dst[len] = 0;
	return dst;
}

/* set a char in a set memory location */
extern int memset(void *m, char c, size_t n) {

	/* loop */
	for (size_t i = 0; i < n; i++)
		((char *)m)[i] = c;
}

/* identical */
extern int kstreq(char *a, char *b) {

	ssize_t al = strlen(a);
	ssize_t bl = strlen(b);
	if (al != bl) return 0;
	for (int i = 0; i < al; i++)
		if (a[i] != b[i]) return 0;
	return 1;
}
