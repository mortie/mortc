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
	TOKEN_TYPE_NAME,

	TOKEN_TYPE_ERROR,
	TOKEN_TYPE_NONE
} m_token_type;

typedef struct m_token
{
	m_token_type type;
	union content {
		char *str;
		int num;
	} content;
	int line;
	int column;
	int refs;
} m_token;

const char *m_token_type_name(m_token_type type);

void m_token_init(m_token *tok);
void m_token_set_content(m_token *tok, const char *content, size_t len);
void m_token_ref(m_token *tok);
void m_token_unref(m_token *tok);

#endif
