/* unistd.c implements the functions defined in src/include/unistd.h */
#include "../../include/unistd.h" /* default header */
#include "../../include/filetype.h" /* FILE struct type */
#include "../kprint.h" /* kernel terminal */
#include "../../include/string.h" /* string functions */

/* custom terminal buffer */
char termbuf[BUFSIZ];

/* unistd.c also defines a file list */
FILE _filebuf[16] = {
	{NULL, "r", 0, 0, 0, 1}, /* stdin */
	{(void *)termbuf, "w", 1, 0, 0, 1}, /* stdout */
}; /* defines default files stdin and stdout */
FILE (*__iob)[] = &_filebuf;

/* close a file */
extern int close(int _fd) {

	/* nonsense descriptor (< 0 or > 15) */
	if (_fd < 0 || _fd > 15) {
		
		char *err_str = "error: nonsense file descriptor (< 0 or > 15) passed into function 'close(fd)'\n";

		/* print message (temporarily disabled) */
		write(1, err_str, strlen(err_str));

		return -1; /* unsuccessful */
	}
	
	/* stdin and stdout (closing them is illegal) */
	else if (_fd < 2) {
	
		char *err_str = "error: attempt to write to non-closable file descriptor (fd < 2) with function 'close(fd)'\n";
		
		/* print message (temporarily disabled) */
		write(1, err_str, strlen(err_str));
	
		return -1; /* unsuccessful */
	}
	
	/* otherwise, close file */
	_filebuf[_fd]._is_open = 0;
	
	/* success */
	return 0;
}

/* flush a file buffer */
extern int flush(int _fd) {

	/* nonsense descriptors */
	if (_fd < 0 || _fd > 15) {

                char *err_str = "error: nonsense file descriptor (< 0 or > 15) passed into function 'flush(fd)'\n";

                /* print message (temporarily disabled) */
                write(1, err_str, strlen(err_str));

                return -1; /* unsuccessful */
        }
	
	/* stdout is an exception */
	if (_fd == 1) {
	
		/* print the buffer and clear it */
		kprint(termbuf);
		
		/* just requires setting first value to be null term char */
		termbuf[0] = 0;
	}
	
	/* todo... */
	else {}

	return 0; /* success */
}

/* write from buffer to file buffer */
extern ssize_t write(int _fd, void *_buf, int _n) {

	/* invalid file descriptor */
	if (_fd < 0 || _fd > 15) {

                char *err_str = "error: nonsense file descriptor (< 0 or > 15) passed into function 'write(fd,buf,n)'\n";

                /* print message (temporarily disabled) */
                write(1, err_str, strlen(err_str));

                return 0; /* unsuccessful */
        }
	
	/* not a write mode file */
	else if ((strcmp(_filebuf[_fd]._md, "w")) != 0) {
	
		char *err_str = "error: non-write mode file descriptor passed into 'write(fd,buf,n)'\n";
	
		/* print message (temporarily disabled) */
		write(1, err_str, strlen(err_str));

		return 0; /* unsuccessful */
	}
	
	/* no buffer */
	else if (_filebuf[_fd]._buf == NULL) {
	
		char *err_str = "error: file with non-existant temporary buffer passed into 'write(fd,buf,n)'\n";
	
		/* print message (temporarily disabled) */
		write(1, err_str, strlen(err_str));	

		return 0; /* unsuccessful */
	}
	
	/* clearing the buffer */
	for (ssize_t i = 0; i < _filebuf[_fd]._bufsiz; i++) {
	
		((char *)_filebuf[_fd]._buf)[i] = 0;
	}

	/* buffer size */
	ssize_t _bufsiz = _filebuf[_fd]._bufsiz;
	
	/* write characters to buffer */
	ssize_t px;
	for (px = 0; px < _n; px++) {
	
		/* if we have reached end of buffer */
	//	if ((px % _filebuf[_fd]._bufsiz) == 0) {
		
			/* flush buffer */
			//flush(_fd);
		
			/* clear buffer */
	//		for (int i = 0; i < _filebuf[_fd]._bufsiz; i++)
	//			((char *)_filebuf[_fd]._buf)[i] = 0;
	//	}

		/* set char in buffer */
		((char *)(_filebuf[_fd]._buf))[px] = ((char *)_buf)[px];
	}
	
	/* set null term char (mainly for stdout) */
	((char *)_filebuf[_fd]._buf)[px] = 0;
	
	/* flush buffer for final time */
	flush(_fd);
	
	/* success */
	return px;
}
