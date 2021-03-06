#ifndef STREAM_H
#define STREAM_H

#include "mortc/token.h"

#include <stdio.h>

#define STREAM_CHAR_LOOKAHEAD 1
#define STREAM_TOKEN_LOOKAHEAD 1

enum m_stream_type
{
	STREAM_TYPE_STRING,
	STREAM_TYPE_FILE
};

typedef struct m_stream
{
	int line;
	int column;
	size_t character;
	int reached_eof;
	int indents;

	enum m_stream_type type;
	union {
		char *str;
		FILE *f;
	} input;

	int nextchar[STREAM_CHAR_LOOKAHEAD];
	int ch;

	m_token nexttoken[STREAM_TOKEN_LOOKAHEAD];
	m_token token;

	struct {
		void **vec;
		size_t size;
		size_t length;
	} allocs;
} m_stream;

void m_stream_init_str(m_stream *stream, char *str);
void m_stream_init_file(m_stream *stream, FILE *f);
void m_stream_read_token(m_stream *stream);
void m_stream_free(m_stream *stream);

// Check if the current token is of the desired type.
// Return 0, or -1 on error.
int m_stream_assert(m_stream *stream, m_token_type type);

// Skip a token if it's of the desired type.
// Return 0, or -1 on error.
int m_stream_skip(m_stream *stream, m_token_type type);

// Check if the current token is one of the desired types.
// Return 0, or -1 on error. 'types' is an array terminated by TOKEN_TYPE_NONE.
int m_stream_assert_any(m_stream *stream, m_token_type types[]);

// Skip a token if it's of one of the desired types.
// Return 0, or -1 on error. 'types' is an array terminated by TOKEN_TYPE_NONE.
int m_stream_skip_any(m_stream *stream, m_token_type types[]);

// Skip a token if it's of the desired type.
// Nothing is done if the next token isn't of the desired type.
void m_stream_optional(m_stream *stream, m_token_type type);

#endif
