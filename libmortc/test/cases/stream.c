#include "test.h"
#include "mortc/stream.h"

#include <stdio.h>

void test_stream()
{
	it("reads in text") {
		m_stream s;
		m_stream_init_str(&s, "afternoon");

		asserteq(s.token->type, TOKEN_TYPE_NAME);
		assertstr(s.token->content.str, "afternoon");

		m_stream_free(&s);
	}

	it("reads in files") {
		m_stream s;
		FILE *f = fopen("assets/stream_1", "r");
		m_stream_init_file(&s, f);

		asserteq(s.token->type, TOKEN_TYPE_NAME);
		assertstr(s.token->content.str, "afternoon");

		fclose(f);
		m_stream_free(&s);
	}

	it("reads a string of tokens") {
		char *str = "\"hello\" () whats-up";
		m_stream s;
		m_stream_init_str(&s, str);

		asserteq(s.token->type, TOKEN_TYPE_STRING);
		assertstr(s.token->content.str, "hello");

		m_stream_read_token(&s);
		asserteq(s.token->type, TOKEN_TYPE_OPENPAREN);

		m_stream_read_token(&s);
		asserteq(s.token->type, TOKEN_TYPE_CLOSEPAREN);

		m_stream_read_token(&s);
		asserteq(s.token->type, TOKEN_TYPE_NAME);
		assertstr(s.token->content.str, "whats-up");

		m_stream_read_token(&s);
		asserteq(s.token->type, TOKEN_TYPE_EOF);

		m_stream_free(&s);
	}
}
