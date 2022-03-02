#include "../../include/string.h" /* stdlib string functions */

/* return length of string in chars */
extern ssize_t strlen(char *s) {

	ssize_t res; /* variable to hold result */
	for (res = 0; *s++; res++) {} /* get string length */
	return res; /* return value */
}

/* compare strings */
extern int strcmp(char *a, char *b) {

	/* loop and compare */
	while ((*a && *b) && (*a++ == *b++)) {}
	
	/* return difference between last chars */
	return (int)(*(unsigned char *)a) - (int)(*(unsigned char *)b);
}

/* copy n bytes from a to b */
extern void *memcpy(void *b, void *a, size_t n) {

	/* loop */
	for (size_t i = 0; i < n; i++)
		((char *)b)[i] = ((char *)a)[i];
}

/* strcpy */
extern char *strcpy(char *dst, char *src) {

	memcpy(dst, src, strlen(src));
	dst[strlen(src)] = 0;
}

/* set a char in a set memory location */
extern int memset(void *m, char c, size_t n) {

	/* loop */
	for (size_t i = 0; i < n; i++)
		((char *)m)[i] = c;
}
