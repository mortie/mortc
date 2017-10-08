#include "mortc/stream.h"

#include <string.h>
#include <ctype.h>

static int stream_read_char(m_stream *stream)
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

static void stream_read_single_token(m_stream *stream, m_token *tok)
{
	m_token_init(tok);
	m_token_ref(tok);

	int ch = stream->nextchar[0];
	tok->line = stream->line;
	tok->column = stream->column;

#define chartok(c, t) if (ch == c) \
	{ tok->type = t; stream_read_char(stream); return; }
	chartok(EOF, TOKEN_TYPE_EOF)
	chartok('.', TOKEN_TYPE_PERIOD)
	chartok(',', TOKEN_TYPE_COMMA)
	chartok('(', TOKEN_TYPE_OPENPAREN)
	chartok(')', TOKEN_TYPE_CLOSEPAREN)
	chartok('{', TOKEN_TYPE_OPENBRACE)
	chartok('}', TOKEN_TYPE_CLOSEBRACE)
	chartok('[', TOKEN_TYPE_OPENBRACKET)
	chartok(']', TOKEN_TYPE_CLOSEBRACKET)
#undef chartok

	// ':', ':=', '::'
	if (ch == ':')
	{
		int nxt = stream->nextchar[1];
		if (nxt == ':')
		{
			tok->type = TOKEN_TYPE_DBLCOLON;
			stream_read_char(stream);
			return;
		}
		else if (nxt == '=')
		{
			tok->type = TOKEN_TYPE_COLONEQ;
			stream_read_char(stream);
			return;
		}
		else
		{
			tok->type = TOKEN_TYPE_COLON;
			return;
		}
	}

	// Skip whitespace, and insert newstatm if appropriate
	if (isspace(ch))
	{
		int indents = 0;
		do
		{
			if (ch == '\n')
				indents = 0;
			else
				indents += 1;
			ch = stream_read_char(stream);
		} while (isspace(ch));

		if (indents <= stream->indents)
		{
			stream->indents = indents;
			tok->type = TOKEN_TYPE_STATMTERM;
			tok->content.num = indents;
		}
		else
		{
			// Read once again
			stream_read_single_token(stream, tok);
		}

		return;
	}

	// String
	if (ch == '"')
	{
		char *buf = stream->tmpbuf;
		size_t buflen = sizeof(stream->tmpbuf);
		int length = 0;

		ch = stream_read_char(stream);
		while (ch != '"' && ch != EOF)
		{
			if (length >= buflen)
			{
				buflen *= 2;
				if (buf == stream->tmpbuf)
					buf = malloc(buflen);
				else
					buf = realloc(buf, buflen);
			}
			buf[length++] = ch;
			ch = stream_read_char(stream);
		}

		if (ch == EOF)
		{
			tok->type = TOKEN_TYPE_ERROR;
			char str[] = "Reached EOF while parsing string.";
			m_token_set_content(tok, str, sizeof(str));
			if (buf != stream->tmpbuf)
				free(buf);
			return;
		}

		tok->type = TOKEN_TYPE_STRING;
		m_token_set_content(tok, buf, length);
		if (buf != stream->tmpbuf)
			free(buf);

		stream_read_char(stream);

		return;
	}

	// Number
	if (isdigit(ch))
	{
		char *buf = stream->tmpbuf;
		size_t buflen = sizeof(stream->tmpbuf);
		int length = 0;

		int haspoint = 0;
		do
		{
			if (length >= buflen)
			{
				buflen *= 2;
				if (buf == stream->tmpbuf)
					buf = malloc(buflen);
				else
					buf = realloc(buf, buflen);
			}

			if (ch == '.')
				haspoint = 1;

			buf[length++] = ch;
			ch = stream_read_char(stream);
		} while (isdigit(ch) || (ch == '.' && !haspoint));

		tok->type = TOKEN_TYPE_NUMBER;
		m_token_set_content(tok, buf, length);
		if (buf != stream->tmpbuf)
			free(buf);

		return;
	}

	// Name
	if (!isspace(ch) && ch != EOF &&
			ch != '(' && ch != ')' &&
			ch != '[' && ch != ']' &&
			ch != '{' && ch != '}' &&
			ch != ',' && ch != '.' &&
			ch != '"')
	{
		char *buf = stream->tmpbuf;
		size_t buflen = sizeof(stream->tmpbuf);
		int length = 0;

		do
		{
			if (length >= buflen)
			{
				buflen *= 2;
				if (buf == stream->tmpbuf)
					buf = malloc(buflen);
				else
					buf = realloc(buf, buflen);
			}

			buf[length++] = ch;
			ch = stream_read_char(stream);
		} while (!isspace(ch) && ch != EOF &&
				ch != '(' && ch != ')' &&
				ch != '[' && ch != ']' &&
				ch != '{' && ch != '}' &&
				ch != ',' && ch != '.' &&
				ch != '"');

		tok->type = TOKEN_TYPE_NAME;
		m_token_set_content(tok, buf, length);
		if (buf != stream->tmpbuf)
			free(buf);

		return;
	}

	tok->type = TOKEN_TYPE_ERROR;
	char err[] = "Unexpected character: 'x'";
	err[sizeof(err) - 3] = ch;
	m_token_set_content(tok, err, sizeof(err));
}

static void stream_init(m_stream *stream)
{
	stream->line = 1;
	stream->column = 1;
	stream->character = 0;
	stream->indents = 0;

	memset(stream->nextchar, 0, sizeof(stream->nextchar));
	memset(stream->nexttoken, 0, sizeof(stream->nexttoken));

	for (int i = 0; i < STREAM_CHAR_LOOKAHEAD; ++i)
		stream_read_char(stream);

	for (int i = 0; i < STREAM_TOKEN_LOOKAHEAD; ++i)
		m_token_init(&stream->nexttoken[i]);
	for (int i = 0; i < STREAM_TOKEN_LOOKAHEAD; ++i)
		m_stream_read_token(stream);
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

void m_stream_free(m_stream *stream)
{
	for (int i = 0; i < STREAM_TOKEN_LOOKAHEAD; ++i)
		m_token_unref(&stream->nexttoken[i]);
}

void m_stream_read_token(m_stream *stream)
{
	m_token nxt;
	stream_read_single_token(stream, &nxt);
	if (nxt.type == TOKEN_TYPE_ERROR)
		fprintf(stderr, "Error: %s\n", nxt.content.str);

	m_token_unref(&stream->nexttoken[0]);
	for (int i = STREAM_TOKEN_LOOKAHEAD - 2; i >= 0; --i)
		stream->nexttoken[i] = stream->nexttoken[i + 1];
	stream->nexttoken[STREAM_CHAR_LOOKAHEAD - 1] = nxt;
	stream->token = &stream->nexttoken[0];;
}

int m_stream_skip(m_stream *stream, m_token_type type)
{
	if (stream->token->type == type)
	{
		m_stream_read_token(stream);
		return 0;
	}
	else
	{
		return -1;
	}
}
