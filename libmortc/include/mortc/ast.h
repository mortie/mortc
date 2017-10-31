#ifndef PARSER_H
#define PARSER_H

#include "stream.h"

/*
 * The m_ast_* structs represent a piece of the mortc grammar.
 * The m_ast_*_parse functions parse their respective grammar into the
 * struct pointed to by the first argument. The functions might allocate
 * memory with malloc. They return 0 ond success, and -1 on error.
 * The m_ast_*_free functions recursively free memory allocated by
 * m_ast_*_parse.
 */

// Forward declarationi
typedef struct m_ast_expr_number m_ast_expr_number;
typedef struct m_ast_expr_string m_ast_expr_string;
typedef struct m_ast_expr_name m_ast_expr_name;
typedef struct m_ast_expr_group m_ast_expr_group;
typedef struct m_ast_expr_func_call m_ast_expr_func_call;
typedef struct m_ast_expr m_ast_expr;
typedef struct m_ast_statm m_ast_statm;
typedef struct m_ast_block m_ast_block;
typedef struct m_ast_type m_ast_type;
typedef struct m_ast_vardecl m_ast_vardecl;
typedef struct m_ast_funcdecl m_ast_funcdecl;
typedef struct m_ast_decl m_ast_decl;
typedef struct m_ast_file m_ast_file;

/*
 * Number Expression
 * <string>
 */

typedef struct m_ast_expr_number
{
	union {
		int i;
		double d;
	} n;
	enum {
		AST_EXPR_NUMBER_INTEGER,
		AST_EXPR_NUMBER_DOUBLE
	} tag;
} m_ast_expr_number;

int m_ast_expr_number_parse(m_ast_expr_number *ast, m_stream *stream);
void m_ast_expr_number_free(m_ast_expr_number *ast);

/*
 * String Expression
 * <string>
 */

typedef struct m_ast_expr_string
{
	char *str;
} m_ast_expr_string;

int m_ast_expr_string_literal_parse(m_ast_expr_name *ast, m_stream *stream);
void m_ast_expr_string_literal_free(m_ast_expr_name *ast);

/*
 * Name Expression
 * <name>
 */

typedef struct m_ast_expr_name
{
	char *name;
} m_ast_expr_name;

int m_ast_expr_name_parse(m_ast_expr_name *ast, m_stream *stream);
void m_ast_expr_name_free(m_ast_expr_name *ast);

/*
 * Func Call Expression
 * '('? <name> (<number> | <string> | <name> | <func call>)* ')'?
 */

typedef struct m_ast_expr_func_call
{
	char *name;
	m_ast_expr *args;
	size_t args_len;
	size_t args_size;
} m_ast_expr_func_call;

int m_ast_expr_func_call_parse(m_ast_expr_func_call *ast, m_stream *stream);
void m_ast_expr_func_call_free(m_ast_expr_func_call *ast);

/*
 * Expression
 * <name> | <func call>
 */

typedef struct m_ast_expr
{
	union
	{
		m_ast_expr_string string;
		m_ast_expr_number number;
		m_ast_expr_name name;
		m_ast_expr_func_call func_call;
	} expr;

	enum
	{
		AST_EXPR_STRING,
		AST_EXPR_NUMBER,
		AST_EXPR_NAME,
		AST_EXPR_FUNC_CALL,
		AST_EXPR_NONE
	} tag;
} m_ast_expr;

int m_ast_expr_parse(m_ast_expr *ast, m_stream *stream);
void m_ast_expr_free(m_ast_expr *ast);

/*
 * Statement
 */

typedef struct m_ast_statm
{
	union
	{
		int foo;
	} statm;

	enum
	{
		AST_STATM_FOO
	} tag;
} m_ast_statm;

int m_ast_statm_parse(m_ast_expr *ast, m_stream *stream);
void m_ast_statm_free(m_ast_expr *ast);

/*
 * Block
 * '{' <vardecl>* <statm>* '}'
 */

typedef struct m_ast_block
{
	m_ast_statm *statms;
	size_t statms_len;
	size_t statms_size;
} m_ast_block;

int m_ast_block_parse(m_ast_block *ast, m_stream *stream);
void m_ast_block_free(m_ast_block *ast);

/*
 * Type Declaration
 * <name>
 */

typedef struct m_ast_type
{
	char *name;
} m_ast_type;

int m_ast_typedecl_parse(m_ast_type *ast, m_stream *stream);
void m_ast_typedecl_free(m_ast_type *ast);

/*
 * Variable Declaration
 * <typedecl> <name>
 */

typedef struct m_ast_vardecl
{
	m_ast_type type;
	char *name;
} m_ast_vardecl;

int m_ast_vardecl_parse(m_ast_vardecl *ast, m_stream *stream);
void m_ast_vardecl_free(m_ast_vardecl *ast);

/*
 * Function Declaration
 * <name> '(' (<vardecl> ',')* ')' (':' <type>)? <block>?
 */

typedef struct m_ast_funcdecl
{
	char *name;

	m_ast_vardecl *args;
	size_t args_len;
	size_t args_size;

	m_ast_type retval;
	m_ast_block block;
} m_ast_funcdecl;

int m_ast_funcdecl_parse(m_ast_funcdecl *ast, m_stream *stream);
void m_ast_funcdecl_free(m_ast_funcdecl *ast);

/*
 * Declaration
 * <var decl> | <func decl>
 */

typedef struct m_ast_decl
{
	union {
		m_ast_vardecl vardecl;
		m_ast_funcdecl funcdecl;
	} decl;

	enum {
		AST_DECL_FUNCDECL,
		AST_DECL_VARDECL
	} tag;
} m_ast_decl;

int m_ast_decl_parse(m_ast_decl *ast, m_stream *stream);
void m_ast_decl_free(m_ast_decl *ast);

/*
 * File
 */

typedef struct m_ast_file
{
	m_ast_decl *decls;
	size_t decls_len;
	size_t decls_size;
} m_ast_file;

int m_ast_file_parse(m_ast_file *ast, m_stream *stream);
void m_ast_file_free(m_ast_file *ast);

#endif
