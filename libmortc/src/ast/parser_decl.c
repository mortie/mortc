#include "mortc/ast.h"
#include "mortc/stream.h"

#include "macros.h"

#include <ctype.h>

int m_ast_typedecl_parse(m_ast_typedecl *ast, m_stream *stream)
{
	ast->parts = NULL;
	ast->parts_len = 0;
	ast->parts_size = 0;

	while (!ENDTOKEN(stream->token.type))
	{
		TRY(m_stream_assert_any(stream, (m_token_type[]) {
			TOKEN_TYPE_NAME,
			TOKEN_TYPE_TYPENAME,
			TOKEN_TYPE_OPENPAREN,
			TOKEN_TYPE_NONE
		}));

		if (stream->token.type == TOKEN_TYPE_NAME)
		{
			// Terminate once we meet a name which doesn't start with a capital
			if (!isupper(stream->token.content.str[0]))
				break;

			struct m_ast_typedecl_part part;
			part.tag = AST_TYPEDECL_NAME;
			part.d.name = stream->token.content.str;
			TRY(m_stream_skip(stream, TOKEN_TYPE_NAME));
			APPEND(ast->parts, ast->parts_len, ast->parts_size, part);
		}
		else if (stream->token.type == TOKEN_TYPE_TYPENAME)
		{
			struct m_ast_typedecl_part part;
			part.tag = AST_TYPEDECL_TYPENAME;
			part.d.name = stream->token.content.str;
			TRY(m_stream_skip(stream, TOKEN_TYPE_TYPENAME));
			APPEND(ast->parts, ast->parts_len, ast->parts_size, part);
		}
		else if (stream->token.type == TOKEN_TYPE_OPENPAREN)
		{
			m_stream_skip(stream, TOKEN_TYPE_OPENPAREN);

			struct m_ast_typedecl_part part;
			part.tag = AST_TYPEDECL_NESTED;
			part.d.decl = malloc(sizeof(*part.d.decl));

			if (m_ast_typedecl_parse(part.d.decl, stream) < 0)
			{
				m_ast_typedecl_free(part.d.decl);
				free(part.d.decl);
				return -1;
			}

			APPEND(ast->parts, ast->parts_len, ast->parts_size, part);

			m_stream_skip(stream, TOKEN_TYPE_CLOSEPAREN);
		}
	}

	return 0;
}

void m_ast_typedecl_free(m_ast_typedecl *ast)
{
	for (size_t i = 0; i < ast->parts_len; ++i)
	{
		if (ast->parts[i].tag == AST_TYPEDECL_NESTED)
		{
			m_ast_typedecl_free(ast->parts[i].d.decl);
			free(ast->parts[i].d.decl);
		}
	}

	free(ast->parts);
}
