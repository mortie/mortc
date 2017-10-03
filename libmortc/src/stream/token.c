#include "mortc/token.h"

#include <string.h>

void m_token_init(m_token *tok, m_token_type type)
{
	tok->type = type;
	tok->content = NULL;
}

void m_token_set_content(m_token *tok, char *content, size_t len)
{
	tok->content = malloc(len + 1);
	memcpy(tok->content, content, len);
	tok->content[len - 1] = '\0';
}
