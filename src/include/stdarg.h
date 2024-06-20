#ifndef STDARG_H
#define STDARG_H

#include "../types.h"

#define _ARG_ALIGN(x) (((x) + 3) & ~(3))

typedef struct {
	void *s; /* start */
	void *p; /* current */
} va_list;

#define va_start(v, n) ({\
		(v).s = &(n) + 1;\
		(v).p = (v).s;\
	})

#define va_arg(v, tp) (*(tp *)(((v).p += _ARG_ALIGN(sizeof(tp))) - _ARG_ALIGN(sizeof(tp))))

#define va_end(v) ({\
		(v).s = NULL;\
		(v).p = NULL;\
	})

#endif /* STDARG_H */
