#include "mortc/stream.h"

#include <string.h>

static int stream_read(m_stream *stream)
{
	if (stream->nextchar[0] == EOF)
		return EOF;

	int nxt;
	if (stream->type == STREAM_TYPE_STRING)
	{
		nxt = stream->input.str[stream->character];
		if (nxt == 0)
			nxt = EOF;
	}
	else
	{
		nxt = fgetc(stream->input.f);
	}

	stream->character += 1;

	for (int i = STREAM_CHAR_LOOKAHEAD - 2; i >= 0; --i)
		stream->nextchar[i] = stream->nextchar[i + 1];
	stream->nextchar[STREAM_CHAR_LOOKAHEAD - 1] = nxt;

	return stream->nextchar[0];
}

static void stream_init(m_stream *stream)
{
	stream->line = 1;
	stream->column = 1;
	stream->character = 0;

	memset(stream->nextchar, 0, sizeof(stream->nextchar));
	memset(stream->nexttoken, 0, sizeof(stream->nexttoken));

	for (int i = 0; i < STREAM_CHAR_LOOKAHEAD; ++i)
		stream_read(stream);
}

void m_stream_init_str(m_stream *stream, char *str)
{
	stream->type = STREAM_TYPE_STRING;
	stream->input.str = str;

	stream_init(stream);
}

void m_stream_init_file(m_stream *stream, FILE *f)
{
	stream->type = STREAM_TYPE_FILE;
	stream->input.f = f;

	stream_init(stream);
}
