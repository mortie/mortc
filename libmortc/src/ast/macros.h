#include "mortc/stream.h"

#define TRY(x) \
	do { \
		if (x < 0) return -1; \
	} while (0)

#define ENDTOKEN(x) ( \
	x == TOKEN_TYPE_CLOSEPAREN || \
	x == TOKEN_TYPE_CLOSEBRACE || \
	x == TOKEN_TYPE_EOF)

#define APPEND(arr, len, size, elem) \
	do { \
		len += 1; \
		if (len >= size) { \
			size = size ? size * 2 : 8; \
			arr = realloc(arr, size * sizeof(elem)); \
		} \
		arr[len - 1] = elem; \
	} while (0)
