#include "mortc/token.h"

#include <string.h>

const char *m_token_type_name(m_token_type type)
{
	switch (type)
	{
	case TOKEN_TYPE_EOF:
		return "end-of-file";
	case TOKEN_TYPE_PERIOD:
		return "period";
	case TOKEN_TYPE_COMMA:
		return "comma";
	case TOKEN_TYPE_OPENPAREN:
		return "open paren";
	case TOKEN_TYPE_CLOSEPAREN:
		return "close paren";
	case TOKEN_TYPE_OPENBRACE:
		return "open brace";
	case TOKEN_TYPE_CLOSEBRACE:
		return "close brace";
	case TOKEN_TYPE_OPENBRACKET:
		return "open bracket";
	case TOKEN_TYPE_CLOSEBRACKET:
		return "close bracket";

	case TOKEN_TYPE_COLON:
		return "colon";
	case TOKEN_TYPE_DBLCOLON:
		return "double colon";
	case TOKEN_TYPE_COLONEQ:
		return "colon equal";

	case TOKEN_TYPE_STATMTERM:
		return "statement terminator";
	case TOKEN_TYPE_STRING:
		return "string";
	case TOKEN_TYPE_NUMBER:
		return "number";
	case TOKEN_TYPE_TYPENAME:
		return "type name";
	case TOKEN_TYPE_NAME:
		return "name";

	case TOKEN_TYPE_ERROR:
		return "error";
	case TOKEN_TYPE_NONE:
		return "none";
	}

	return "unknown";
}

void m_token_init(m_token *tok)
{
	tok->type = TOKEN_TYPE_NONE;
	tok->content.str = NULL;
}
