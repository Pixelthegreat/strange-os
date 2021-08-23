/* standard input and output */
#ifndef _STDIO_H
#define _STDIO_H

/* default io buffer size */
#define BUFSIZ 8192

#define EOF (-1) /* end of file to indicate ya know... end of file */

/* fseek modes */
#define SEEK_SET 0 /* seek from beginning of file */
#define SEEK_CUR 1 /* seek from current position */
#define SEEK_END 2 /* seek from eof position */

/* file numbers */
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

/* io buffer magik */
extern FILE (*__iobf)[]; /* for reference: pointer to a FILE array */
#define _iob (*__iobf) /* the dereferenced version */

/* files */
#define stdin (&_iob[0]) /* stdin -- user input (typically keyboard) */
#define stdout (&_iob[1]) /* stdout -- user output (typically text mode stuff) */
#define stderr (&_iob[2]) /* for errors */

/* file functions */
extern int remove(char *); /* remove a file */
extern int rename(char *, char *); /* rename a file */
extern int fclose(FILE *); /* close a FILE stream */
extern int fflush(FILE *); /* flush a FILE stream */
extern FILE *fopen(char *, char *); /* open a FILE stream */
extern FILE *freopen(char *, char *, FILE *); /* reopen a FILE stream */
extern FILE *fdopen(int, char *); /* open a new FILE stream which refers to existing file descriptor (i.e., stdin2=fdopen(0, "r") */
extern FILE *fmemopen(void *, size_t, char *); /* open a new FILE stream which refers to an existing buffer */

/* printfs */
/* actually need to implement variadic system (i don't have that up yet) */

/* get character from FILE stream */
extern int fgetc(FILE *);
extern int getc(FILE *);

/* read char from stdin */
extern int getchar(void);

/* write char to FILE stream */
extern int fputc(int, FILE *);
extern int putc(int, FILE *);

/* write char to stdout */
extern int putchar(int);

/* get word (int) from stream */
extern int getw(FILE *);

/* write word (int) to stream */
extern int putw(int, FILE *);

/* get a null terminated string from FILE stream */
extern char *fgets(char *, int, FILE *);

/* get a null terminated string from stdin
 * 'gets' was deprecated in ISOC++11, and removed
 * in both ISOC11 and ISOC++14. The function is
 * dangerous under all circumstances, so PLEASE,
 * despite the fact that it has been implemented
 * anyway, DO NOT USE the GETS function.
 */
extern char *gets(char *);

/* write a string to FILE stream */
extern int fputs(char *, FILE *);

/* write a string to stdout */
extern int puts(char *);

/* read data from file STREAM */
extern size_t fread(void *, size_t, size_t, FILE *);

/* write data to STREAM */
extern size_t fwrite(void *, size_t, size_t, FILE *);

/* seek to position in STREAM */
extern int fseek(FILE *, long int, int);

/* return pos of FILE stream */
extern int ftell(FILE *);

/* non-libc function: flength
 * returns length of file
 */
static inline int flength(FILE *_fp) {

	int _pos = ftell(_fp); /* save original position */

	/* go to end */
	fseek(_fp, 0, SEEK_END);
	int res = ftell(_fp); /* final result */
	/* go back to original position */
	fseek(_fp, _pos, SEEK_SET);

	/* return value: length of file */
	return res;
} /* unfinished; refer to /usr/include/stdio.h */

#endif /* _STDIO_H */
