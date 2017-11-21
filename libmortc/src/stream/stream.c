#include "mortc/stream.h"

#include <string.h>
#include <ctype.h>

#define ERRORSTART(tok) \
	fprintf(stderr, "Error: %i:%i: ", (tok)->line, (tok)->column);
#define ERRORCONT(...) \
	fprintf(stderr, __VA_ARGS__)
#define ERROREND() \
	fprintf(stderr, "\n")

/*
 * strvec is a utility structure for dynamically growing strings
 */
struct strvec
{
	char *vec;
	size_t size;
	size_t length;
};
static void strvec_init(struct strvec *str)
{
	str->vec = NULL;
	str->size = 0;
	str->length = 0;
}
static void strvec_append(struct strvec *str, char c)
{
	str->length += 1;
	if (str->length >= str->size)
	{
		if (str->size == 0)
			str->size = 512;
		else
			str->size *= 2;

		str->vec = realloc(str->vec, str->size);
	}

	str->vec[str->length - 1] = c;
	str->vec[str->length] = '\0';
}

static int isname(int ch)
{
	return !isspace(ch) && ch != EOF &&
		ch != '(' && ch != ')' &&
		ch != '[' && ch != ']' &&
		ch != '{' && ch != '}' &&
		ch != ',' && ch != '.' &&
		ch != '"';
}

static void stream_allocs_append(m_stream *stream, void *ptr)
{
	stream->allocs.length += 1;
	if (stream->allocs.length > stream->allocs.size)
	{
		if (stream->allocs.size == 0)
			stream->allocs.size = 512;
		else
			stream->allocs.size *= 2;

		stream->allocs.vec = realloc(
			stream->allocs.vec,
			sizeof(*stream->allocs.vec) * stream->allocs.size);
	}

	stream->allocs.vec[stream->allocs.length - 1] = ptr;
}

static int stream_read_char(m_stream *stream)
{
	if (stream->ch == EOF)
		return EOF;

	int nxt;
	if (stream->reached_eof)
	{
		nxt = EOF;
	}
	else if (stream->type == STREAM_TYPE_STRING)
	{
		nxt = stream->input.str[stream->character];
		if (nxt == 0)
		{
			stream->reached_eof = 1;
			nxt = EOF;
		}
	}
	else
	{
		nxt = fgetc(stream->input.f);
		if (nxt == EOF)
			stream->reached_eof = 1;
	}

	stream->character += 1;

	stream->ch = stream->nextchar[0];
	for (int i = STREAM_CHAR_LOOKAHEAD - 1; i > 0; --i)
		stream->nextchar[i] = stream->nextchar[i + 1];
	stream->nextchar[STREAM_CHAR_LOOKAHEAD - 1] = nxt;

	if (stream->ch == '\n')
		stream->column = 0;
	else if (stream->ch != 0)
		stream->column += 1;

	return stream->ch;
}

static void stream_read_single_token(m_stream *stream, m_token *tok)
{
	m_token_init(tok);

	struct strvec strvec;

	int ch = stream->ch;
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
		int nxt = stream->nextchar[0];
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

	strvec_init(&strvec);

	// String
	if (ch == '"')
	{

		ch = stream_read_char(stream);
		while (ch != '"' && ch != EOF)
		{
			strvec_append(&strvec, ch);
			ch = stream_read_char(stream);
		}

		if (ch == EOF)
		{
			free(strvec.vec);
			tok->type = TOKEN_TYPE_ERROR;
			tok->content.str = "Reached EOF while parsing string.";
			return;
		}

		tok->type = TOKEN_TYPE_STRING;
		tok->content.str = strvec.vec;
		stream_allocs_append(stream, tok->content.str);

		stream_read_char(stream);

		return;
	}

	// Number
	if (isdigit(ch))
	{
		int haspoint = 0;
		do
		{
			if (ch == '.')
				haspoint = 1;

			strvec_append(&strvec, ch);
			ch = stream_read_char(stream);
		} while (isdigit(ch) || (ch == '.' && !haspoint));

		tok->type = TOKEN_TYPE_NUMBER;
		tok->content.str = strvec.vec;
		stream_allocs_append(stream, tok->content.str);

		return;
	}

	// Typename
	if (ch == '\'')
	{
		ch = stream_read_char(stream);

		while (isname(ch))
		{
			strvec_append(&strvec, ch);
			ch = stream_read_char(stream);
		}

		tok->type = TOKEN_TYPE_TYPENAME;
		tok->content.str = strvec.vec;
		stream_allocs_append(stream, tok->content.str);

		return;
	}

	// Name
	if (isname(ch))
	{
		do
		{
			strvec_append(&strvec, ch);
			ch = stream_read_char(stream);
		} while (isname(ch));

		tok->type = TOKEN_TYPE_NAME;
		tok->content.str = strvec.vec;
		stream_allocs_append(stream, tok->content.str);

		return;
	}

	tok->type = TOKEN_TYPE_ERROR;
	tok->content.str = "Unexpected character.";
}

static void stream_init(m_stream *stream)
{
	stream->line = 1;
	stream->column = 0;
	stream->character = 0;
	stream->reached_eof = 0;
	stream->indents = 0;
	stream->allocs.vec = NULL;
	stream->allocs.size = 0;
	stream->allocs.length = 0;
	stream->ch = 0;

	memset(stream->nextchar, 0, sizeof(stream->nextchar));
	memset(stream->nexttoken, 0, sizeof(stream->nexttoken));

	for (int i = 0; i < STREAM_CHAR_LOOKAHEAD + 1; ++i)
		stream_read_char(stream);

	m_token_init(&stream->token);
	for (int i = 0; i < STREAM_TOKEN_LOOKAHEAD + 1; ++i)
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

void m_stream_read_token(m_stream *stream)
{
	m_token nxt;
	stream_read_single_token(stream, &nxt);
	if (nxt.type == TOKEN_TYPE_ERROR)
	{
		ERRORSTART(&nxt);
		ERRORCONT("%s", nxt.content.str);
		ERROREND();
	}

	stream->token = stream->nexttoken[0];
	for (int i = STREAM_TOKEN_LOOKAHEAD - 1; i > 0; --i)
		stream->nexttoken[i] = stream->nexttoken[i + 1];
	stream->nexttoken[STREAM_CHAR_LOOKAHEAD - 1] = nxt;
}

void m_stream_free(m_stream *stream)
{
	for (size_t i = 0; i < stream->allocs.length; ++i)
		free(stream->allocs.vec[i]);

	free(stream->allocs.vec);
}

int m_stream_assert(m_stream *stream, m_token_type type)
{
	if (stream->token.type == type)
	{
		return 0;
	}
	else
	{
		ERRORSTART(&stream->token);
		ERRORCONT("Got '%s' token, expected '%s'.",
			m_token_type_name(type),
			m_token_type_name(stream->token.type));
		ERROREND();
		return -1;
	}
}

int m_stream_skip(m_stream *stream, m_token_type type)
{
	int ret = m_stream_assert(stream, type);
	if (ret >= 0)
		m_stream_read_token(stream);

	return ret;
}

int m_stream_assert_any(m_stream *stream, m_token_type types[])
{
	int valid = 0;
	m_token_type *t = types;
	while (*t != TOKEN_TYPE_NONE)
	{
		if (stream->token.type == *t)
		{
			valid = 1;
			break;
		}
		t += 1;
	}

	if (valid)
	{
		return 0;
	}
	else
	{
		ERRORSTART(&stream->token);
		ERRORCONT("Got '%s' token, expected ",
			m_token_type_name(stream->token.type));
		t = types;
		while (*t != TOKEN_TYPE_NONE)
		{
			const char *name = m_token_type_name(*t);
			if (t == types)
				ERRORCONT("'%s'", name);
			else if (*(t + 1) == TOKEN_TYPE_NONE)
				ERRORCONT(" or '%s'.", name);
			else
				ERRORCONT(", '%s'", name);
			t += 1;
		}
		ERROREND();
		return -1;
	}
}

int m_stream_skip_any(m_stream *stream, m_token_type types[])
{
	int ret = m_stream_assert_any(stream, types);
	if (ret >= 0)
		m_stream_read_token(stream);

	return ret;
}

void m_stream_optional(m_stream *stream, m_token_type type)
{
	if (stream->token.type == type)
		m_stream_read_token(stream);
}
