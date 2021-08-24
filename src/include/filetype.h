/* defines the FILE struct type */
#ifndef _FILETYPE_H
#define _FILETYPE_H

typedef struct _iobuf {
	void *_buf; /* pointer to file buffer (NULL specifies that the buffer is to be flushed after each read or write) */
	char *_md; /* file mode */
	int   _fd; /* file descriptor */
	int   _pos; /* file position */
	int   _bufsiz; /* file buffer size */
	int   _is_open; /* is the file open? */
} FILE;

#endif /* _FILETYPE_H */
