#include "mortc/token.h"

#include <string.h>

void m_token_init(m_token *tok)
{
	tok->type = TOKEN_TYPE_NONE;
	tok->content.str = NULL;
	tok->refs = 0;
}

void m_token_set_content(m_token *tok, const char *content, size_t len)
{
	tok->content.str = malloc(len + 1);
	memcpy(tok->content.str, content, len);
	tok->content.str[len] = '\0';
}

void m_token_ref(m_token *tok)
{
	tok->refs += 1;
}

void m_token_unref(m_token *tok)
{
	tok->refs -= 1;
	if (tok->refs == 0 && tok->type != TOKEN_TYPE_STATMTERM)
		free(tok->content.str);
}
