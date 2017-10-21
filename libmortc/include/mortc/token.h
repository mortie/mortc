#ifndef TOKEN_H
#define TOKEN_H

#include <stdlib.h>

typedef enum m_token_type
{
	TOKEN_TYPE_EOF,
	TOKEN_TYPE_PERIOD,
	TOKEN_TYPE_COMMA,
	TOKEN_TYPE_OPENPAREN,
	TOKEN_TYPE_CLOSEPAREN,
	TOKEN_TYPE_OPENBRACE,
	TOKEN_TYPE_CLOSEBRACE,
	TOKEN_TYPE_OPENBRACKET,
	TOKEN_TYPE_CLOSEBRACKET,

	TOKEN_TYPE_COLON,
	TOKEN_TYPE_DBLCOLON,
	TOKEN_TYPE_COLONEQ,

	TOKEN_TYPE_STATMTERM,
	TOKEN_TYPE_STRING,
	TOKEN_TYPE_NUMBER,
	TOKEN_TYPE_TYPENAME,
	TOKEN_TYPE_NAME,

	TOKEN_TYPE_ERROR,
	TOKEN_TYPE_NONE
} m_token_type;

/*
 * It's the token's creator's responsibility to allocate and free
 * m_token.content, not the user's. The creator is usually hte scanner.
 */
typedef struct m_token
{
	m_token_type type;

	union
	{
		char *str;
		int num;
	} content;

	int line;
	int column;
} m_token;

#define M_TOKEN_IS_STRING(x) (\
	(x) == TOKEN_TYPE_STRING || \
	(x) == TOKEN_TYPE_TYPENAME || \
	(x) == TOKEN_TYPE_NAME || \
	(x) == TOKEN_TYPE_ERROR)

const char *m_token_type_name(m_token_type type);

void m_token_init(m_token *tok);

#endif
