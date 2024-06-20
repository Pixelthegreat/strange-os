/* string functions */
#ifndef _STRING_H
#define _STRING_H

#include "../types.h" /* custom types */

#define LOWER(c) ((c) >= 'A' && (c) <= 'Z'? (c) + 32: (c))
#define UPPER(c) ((c) >= 'a' && (c) <= 'z'? (c) - 32: (c))

/* functions */
#ifndef STDLIB
extern ssize_t strlen(char *); /* return the length of a string in chars */
extern size_t strcmp(char *, char *); /* compare two strings */
extern char *strcpy(char *, char *); /* copy string b to string a */
extern char *strchr(char *s, char c); /* find first c */

extern void *memcpy(void *, void *, size_t); /* copy n bytes from a to b */
extern int memset(void *, char, size_t); /* set memory in a location to char */

extern int kstreq(char *a, char *b); /* check if string is identical to other */

#else
extern ssize_t _std_strlen(const char *);

#define strlen _std_strlen
#endif

#endif /* _STRING_H */
