#include "test.h"
#include "mortc/ast.h"

#include <stdio.h>

#define INITEXPR(ast, str) \
		m_stream s; \
		m_stream_init_str(&s, str); \
		m_ast_expr ast; \
		assertint(m_ast_expr_parse(&ast, &s), 0)

#define END(ast) \
	m_ast_expr_free(&ast); \
	m_stream_free(&s)

void test_expressions()
{
	it("parses string literal expressions") {
		INITEXPR(ast, "\"hello world\"");

		assertint(ast.tag, AST_EXPR_STRING);
		assertstr(ast.expr.string.str, "hello world");

		END(ast);
	}

	it("parses number expressions with integers") {
		INITEXPR(ast, "1337");

		assertint(ast.tag, AST_EXPR_NUMBER);
		assertint(ast.expr.number.tag, AST_EXPR_NUMBER_INTEGER);
		assertint(ast.expr.number.n.i, 1337);

		END(ast);
	}

	it("parses number expressions with doubles") {
		INITEXPR(ast, "1337.5");

		assertint(ast.tag, AST_EXPR_NUMBER);
		assertint(ast.expr.number.tag, AST_EXPR_NUMBER_DOUBLE);
		assertdbl(ast.expr.number.n.d, 1337.5);

		END(ast);
	}

	it("parses name expressions") {
		INITEXPR(ast, "foo");

		assertint(ast.tag, AST_EXPR_NAME);
		assertstr(ast.expr.name.name, "foo");

		m_ast_expr_free(&ast);
		m_stream_free(&s);
	}

	it("parses func call expressions with no arguments") {
		INITEXPR(ast, "(foobar)");

		assertint(ast.tag, AST_EXPR_FUNC_CALL);
		m_ast_expr_func_call fc = ast.expr.func_call;
		assertstr(fc.name, "foobar");
		assertint(fc.args_len, 0);

		END(ast);
	}

	it("parses func call expressions with arguments") {
		INITEXPR(ast, "foo x y z");

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

		END(ast);
	}

	it("parses func call expressions with arguments in parens") {
		INITEXPR(ast, "(foo x y z)");

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

		END(ast);
	}

	it("parses nested func calls") {
		INITEXPR(ast, "foo (bar x) (baz (x))");

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

		END(ast);
	}

	it("parses function calls with different argument types") {
		INITEXPR(ast, "foo 10 \"hello\" there");

		assertint(ast.tag, AST_EXPR_FUNC_CALL);
		m_ast_expr_func_call fc = ast.expr.func_call;
		assertstr(fc.name, "foo");
		assertint(fc.args_len, 3);

		assertint(fc.args[0].tag, AST_EXPR_NUMBER);
		assertint(fc.args[0].expr.number.tag, AST_EXPR_NUMBER_INTEGER);
		assertint(fc.args[0].expr.number.n.i, 10);

		assertint(fc.args[1].tag, AST_EXPR_STRING);
		assertstr(fc.args[1].expr.string.str, "hello");

		assertint(fc.args[2].tag, AST_EXPR_NAME);
		assertstr(fc.args[2].expr.name.name, "there");

		END(ast);
	}
}

void test_parser()
{
	run(test_expressions);
}
