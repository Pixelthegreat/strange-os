/* string functions */
#ifndef _STRING_H
#define _STRING_H

#include "../types.h" /* custom types */

/* functions */
extern ssize_t strlen(char *); /* return the length of a string in chars */
extern int strcmp(char *, char *); /* compare two strings */
extern char *strcpy(char *, char *); /* copy string b to string a */

extern void *memcpy(void *, void *, size_t); /* copy n bytes from a to b */
extern int memset(void *, char, size_t); /* set memory in a location to char */

#endif /* _STRING_H */
