#include "test.h"
#include "mortc/ast.h"

#include <stdio.h>

void test_parser()
{
	it("parses name expressions") {
		m_stream s;
		m_stream_init_str(&s, "foo");
		m_ast_expr ast;
		assertint(m_ast_expr_parse(&ast, &s), 0);

		assertint(ast.tag, AST_EXPR_NAME);
		assertstr(ast.expr.name.name, "foo");

		m_ast_expr_free(&ast);
		m_stream_free(&s);
	}

	it("parses func call expressions with no arguments") {
		m_stream s;
		m_stream_init_str(&s, "(foobar)");
		m_ast_expr ast;
		assertint(m_ast_expr_parse(&ast, &s), 0);

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
		assertint(m_ast_expr_parse(&ast, &s), 0);

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

	it("parses func call expressions with arguments in parens") {
		m_stream s;
		m_stream_init_str(&s, "(foo x y z)");
		m_ast_expr ast;
		assertint(m_ast_expr_parse(&ast, &s), 0);

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

	it("parses nested func calls") {
		m_stream s;
		m_stream_init_str(&s, "foo (bar x) (baz (x))");
		m_ast_expr ast;
		assertint(m_ast_expr_parse(&ast, &s), 0);

		assertint(ast.tag, AST_EXPR_FUNC_CALL);
		m_ast_expr_func_call fc = ast.expr.func_call;
		assertstr(fc.name, "foo");
		assertint(fc.args_len, 2);

		assertint(fc.args[0].tag, AST_EXPR_FUNC_CALL);
		m_ast_expr_func_call arg0 = fc.args[0].expr.func_call;
		assertstr(arg0.name, "bar");
		assertint(arg0.args_len, 1);
		assertint(arg0.args[0].tag, AST_EXPR_NAME);
		assertstr(arg0.args[0].expr.name.name, "x");

		assertint(fc.args[1].tag, AST_EXPR_FUNC_CALL);
		m_ast_expr_func_call arg1 = fc.args[1].expr.func_call;
		assertstr(arg1.name, "baz");
		assertint(arg1.args_len, 1);
		assertint(arg1.args[0].tag, AST_EXPR_FUNC_CALL);
		assertstr(arg1.args[0].expr.func_call.name, "x");
		assertint(arg1.args[0].expr.func_call.args_len, 0);

		m_ast_expr_free(&ast);
		m_stream_free(&s);
	}
}
