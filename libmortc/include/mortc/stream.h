#ifndef STREAM_H
#define STREAM_H

#include "mortc/token.h"

#include <stdio.h>

#define STREAM_CHAR_LOOKAHEAD 2
#define STREAM_TOKEN_LOOKAHEAD 2

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
	int indents;

	enum m_stream_type type;
	union {
		char *str;
		FILE *f;
	} input;

	int nextchar[STREAM_CHAR_LOOKAHEAD];
	m_token nexttoken[STREAM_TOKEN_LOOKAHEAD];
	m_token *token;

	// Temporary buffer for token content
	char tmpbuf[512];
} m_stream;

void m_stream_init_str(m_stream *stream, char *str);
void m_stream_init_file(m_stream *stream, FILE *f);
void m_stream_free(m_stream *stream);
void m_stream_read_token(m_stream *stream);

int m_stream_skip(m_stream *stream, m_token_type type);

#endif
