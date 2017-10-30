#include "test.h"
#include "mortc/ast.h"

#include <stdio.h>

void test_parser()
{
	it("parses name expressions") {
		m_stream s;
		m_stream_init_str(&s, "foo");
		m_ast_expr ast;
		m_ast_expr_parse(&ast, &s);

		assertint(ast.tag, AST_EXPR_FUNC_CALL);
		assertstr(ast.expr.name.name, "foo");

		m_ast_expr_free(&ast);
		m_stream_free(&s);
	}

	it("parses group expressions") {
		m_stream s;
		m_stream_init_str(&s, "(foo)");
		m_ast_expr ast;
		m_ast_expr_parse(&ast, &s);

		assertint(ast.tag, AST_EXPR_GROUP);
		m_ast_expr_group gr = ast.expr.group;
		assertint(gr.expr->tag, AST_EXPR_NAME);
		assertstr(gr.expr->expr.name.name, "foo");

		m_ast_expr_free(&ast);
		m_stream_free(&s);
	}

	it("parses func call expressions with no arguments") {
		m_stream s;
		m_stream_init_str(&s, "foobar()");
		m_ast_expr ast;
		m_ast_expr_parse(&ast, &s);

		assertint(ast.tag, AST_EXPR_FUNC_CALL);
		m_ast_expr_func_call fc = ast.expr.func_call;
		assertstr(fc.name, "foobar");
		assertint(fc.args_len, 0);

		m_ast_expr_free(&ast);
		m_stream_free(&s);
	}

	it("parses func call expressions with arguments") {
		m_stream s;
		m_stream_init_str(&s, "foo x y z");
		m_ast_expr ast;
		m_ast_expr_parse(&ast, &s);

		assertint(ast.tag, AST_EXPR_FUNC_CALL);
		m_ast_expr_func_call fc = ast.expr.func_call;
		assertstr(fc.name, "foo");
		assertint(fc.args_len, 3);

		assertint(fc.args[0].tag, AST_EXPR_NAME);
		assertstr(fc.args[0].expr.name.name, "x");
		assertint(fc.args[1].tag, AST_EXPR_NAME);
		assertstr(fc.args[1].expr.name.name, "y");
		assertint(fc.args[2].tag, AST_EXPR_NAME);
		assertstr(fc.args[2].expr.name.name, "z");

		m_ast_expr_free(&ast);
		m_stream_free(&s);
	}
}
