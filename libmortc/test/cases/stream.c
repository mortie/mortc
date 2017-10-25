#include "test.h"
#include "mortc/stream.h"

#include <stdio.h>

void test_stream()
{
	it("reads in text") {
		m_stream s;
		m_stream_init_str(&s, "afternoon");

		assertint(s.token.type, TOKEN_TYPE_NAME);
		assertstr(s.token.content.str, "afternoon");

		m_stream_free(&s);
	}

	it("reads in files") {
		m_stream s;
		FILE *f = fopen("assets/stream_1", "r");
		m_stream_init_file(&s, f);

		assertint(s.token.type, TOKEN_TYPE_NAME);
		assertstr(s.token.content.str, "afternoon");

		fclose(f);
		m_stream_free(&s);
	}

	it("reads a string of tokens") {
		char *str = "\"hello\" () whats-up 'tokentype";
		m_stream s;
		m_stream_init_str(&s, str);

		assertint(s.token.type, TOKEN_TYPE_STRING);
		assertstr(s.token.content.str, "hello");

		m_stream_read_token(&s);
		assertint(s.token.type, TOKEN_TYPE_OPENPAREN);

		m_stream_read_token(&s);
		assertint(s.token.type, TOKEN_TYPE_CLOSEPAREN);

		m_stream_read_token(&s);
		assertint(s.token.type, TOKEN_TYPE_NAME);
		assertstr(s.token.content.str, "whats-up");

		m_stream_read_token(&s);
		assertint(s.token.type, TOKEN_TYPE_TYPENAME);
		assertstr(s.token.content.str, "tokentype");

		m_stream_read_token(&s);
		assertint(s.token.type, TOKEN_TYPE_EOF);

		m_stream_free(&s);
	}

	it("skips a single token") {
		char *str = "a,";
		m_stream s;
		m_stream_init_str(&s, str);

		assertint(m_stream_skip(&s, TOKEN_TYPE_NAME), 0);
		assertint(m_stream_skip(&s, TOKEN_TYPE_COMMA), 0);
		assertint(s.token.type, TOKEN_TYPE_EOF);

		m_stream_free(&s);
	}

	it("optionally skips a token") {
		char *str = "a";
		m_stream s;
		m_stream_init_str(&s, str);

		m_stream_optional(&s, TOKEN_TYPE_COMMA);
		assertint(m_stream_skip(&s, TOKEN_TYPE_NAME), 0);
		assertint(s.token.type, TOKEN_TYPE_EOF);

		m_stream_free(&s);
	}

	it("skips any of an array of tokens") {
		char *str = "a,.";
		m_stream s;
		m_stream_init_str(&s, str);

		m_token_type types[] = {
			TOKEN_TYPE_NAME,
			TOKEN_TYPE_COMMA,
			TOKEN_TYPE_PERIOD
		};

		assertint(m_stream_skip_any(&s, types), 0);
		assertint(m_stream_skip_any(&s, types), 0);
		assertint(m_stream_skip_any(&s, types), 0);
		assertint(s.token.type, TOKEN_TYPE_EOF);

		m_stream_free(&s);
	}
}
