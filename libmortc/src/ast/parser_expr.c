#include "mortc/ast.h"
#include "mortc/stream.h"

#include "macros.h"

/*
 * Number Expression
 */

int m_ast_expr_number_parse(m_ast_expr_number *ast, m_stream *stream)
{
	ast->n.i = 0;
	ast->tag = 0;

	m_token tok = stream->token;
	TRY(m_stream_skip(stream, TOKEN_TYPE_NUMBER));

	char *str = tok.content.str;

	int hasdot = 0;
	for (int i = 0; str[i]; ++i)
	{
		if (str[i] == '.')
		{
			hasdot = 1;
			break;
		}
	}

	if (hasdot)
	{
		ast->tag = AST_EXPR_NUMBER_DOUBLE;
		ast->n.d = atof(str);
	}
	else
	{
		ast->tag = AST_EXPR_NUMBER_INTEGER;
		ast->n.i = atoi(str);
	}

	return 0;
}

void m_ast_expr_number_free(m_ast_expr_number *ast) {}

/*
 * String Expression
 */

int m_ast_expr_string_parse(m_ast_expr_string *ast, m_stream *stream)
{
	ast->str = NULL;

	m_token tok = stream->token;
	TRY(m_stream_skip(stream, TOKEN_TYPE_STRING));
	ast->str = tok.content.str;

	return 0;
}

void m_ast_expr_string_free(m_ast_expr_string *ast) {}

/*
 * Name Expression
 */

int m_ast_expr_name_parse(m_ast_expr_name *ast, m_stream *stream)
{
	ast->name = NULL;

	m_token tok = stream->token;
	TRY(m_stream_skip(stream, TOKEN_TYPE_NAME));
	ast->name = tok.content.str;

	return 0;
}

void m_ast_expr_name_free(m_ast_expr_name *ast) {}

/*
 * Func Call Expression
 */

int m_ast_expr_func_call_parse(m_ast_expr_func_call *ast, m_stream *stream)
{
	ast->name = NULL;
	ast->args = NULL;
	ast->args_len = 0;
	ast->args_size = 0;

	int hasparens = 0;

	// '('?
	m_token tok = stream->token;
	if (tok.type == TOKEN_TYPE_OPENPAREN)
	{
		hasparens = 1;
		m_stream_read_token(stream);
	}

	// <name>
	tok = stream->token;
	TRY(m_stream_skip(stream, TOKEN_TYPE_NAME));
	ast->name = tok.content.str;

	// (<name> | <func call>)*
	while (!ENDTOKEN(stream->token.type))
	{
		TRY(m_stream_assert_any(stream, (m_token_type[]) {
			TOKEN_TYPE_NUMBER,
			TOKEN_TYPE_STRING,
			TOKEN_TYPE_OPENPAREN,
			TOKEN_TYPE_NAME,
			TOKEN_TYPE_NONE
		}));

		if (stream->token.type == TOKEN_TYPE_NUMBER)
		{
			m_ast_expr expr;
			expr.tag = AST_EXPR_NUMBER;
			TRY(m_ast_expr_number_parse(&expr.expr.number, stream));
			APPEND(ast->args, ast->args_len, ast->args_size, expr);
		}
		else if (stream->token.type == TOKEN_TYPE_STRING)
		{
			m_ast_expr expr;
			expr.tag = AST_EXPR_STRING;
			TRY(m_ast_expr_string_parse(&expr.expr.string, stream));
			APPEND(ast->args, ast->args_len, ast->args_size, expr);
		}
		else if (stream->token.type == TOKEN_TYPE_OPENPAREN)
		{
			m_ast_expr expr;
			expr.tag = AST_EXPR_FUNC_CALL;
			TRY(m_ast_expr_func_call_parse(&expr.expr.func_call, stream));
			APPEND(ast->args, ast->args_len, ast->args_size, expr);
		}

		else if (stream->token.type == TOKEN_TYPE_NAME)
		{
			m_ast_expr expr;
			expr.tag = AST_EXPR_NAME;
			TRY(m_ast_expr_name_parse(&expr.expr.name, stream));
			APPEND(ast->args, ast->args_len, ast->args_size, expr);
		}
	}

	// ')'?
	if (hasparens)
		TRY(m_stream_skip(stream, TOKEN_TYPE_CLOSEPAREN));

	return 0;
}

void m_ast_expr_func_call_free(m_ast_expr_func_call *ast)
{
	for (size_t i = 0; i < ast->args_len; ++i)
		m_ast_expr_free(ast->args + i);
	free(ast->args);
}

/*
 * Expression
 */

int m_ast_expr_parse(m_ast_expr *ast, m_stream *stream)
{
	ast->tag = AST_EXPR_NONE;

	TRY(m_stream_assert_any(stream, (m_token_type[]) {
		TOKEN_TYPE_NUMBER,
		TOKEN_TYPE_STRING,
		TOKEN_TYPE_OPENPAREN,
		TOKEN_TYPE_NAME,
		TOKEN_TYPE_NONE
	}));

	// Number
	if (stream->token.type == TOKEN_TYPE_NUMBER)
	{
		ast->tag = AST_EXPR_NUMBER;
		TRY(m_ast_expr_number_parse(&ast->expr.number, stream));
		return 0;
	}

	// String
	else if (stream->token.type == TOKEN_TYPE_STRING)
	{
		ast->tag = AST_EXPR_STRING;
		TRY(m_ast_expr_string_parse(&ast->expr.string, stream));
		return 0;
	}

	// Func call with parens
	else if (stream->token.type == TOKEN_TYPE_OPENPAREN)
	{
		ast->tag = AST_EXPR_FUNC_CALL;
		TRY(m_ast_expr_func_call_parse(&ast->expr.func_call, stream));
		return 0;
	}

	// Func call without parens
	else if (
			(stream->token.type == TOKEN_TYPE_NAME) &&
			(!ENDTOKEN(stream->nexttoken[0].type)))
	{
		ast->tag = AST_EXPR_FUNC_CALL;
		TRY(m_ast_expr_func_call_parse(&ast->expr.func_call, stream));
		return 0;
	}

	// Name
	else if (stream->token.type == TOKEN_TYPE_NAME)
	{
		ast->tag = AST_EXPR_NAME;
		TRY(m_ast_expr_name_parse(&ast->expr.name, stream));
		return 0;
	}

	return -1;
}


void m_ast_expr_free(m_ast_expr *ast)
{
	switch (ast->tag)
	{
	case AST_EXPR_STRING:
		m_ast_expr_number_free(&ast->expr.number);
		break;
	case AST_EXPR_NUMBER:
		m_ast_expr_string_free(&ast->expr.string);
		break;
	case AST_EXPR_NAME:
		m_ast_expr_name_free(&ast->expr.name);
		break;
	case AST_EXPR_FUNC_CALL:
		m_ast_expr_func_call_free(&ast->expr.func_call);
		break;
	case AST_EXPR_NONE:
		break;
	}
}
