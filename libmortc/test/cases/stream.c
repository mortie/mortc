#include "test.h"
#include "mortc/stream.h"

#include <stdio.h>

void test_stream()
{
	it("reads in text") {
		m_stream s;
		m_stream_init_str(&s, "afternoon");

		asserteq(s.nextchar[0], 'a');
		asserteq(s.nextchar[1], 'f');
		asserteq(s.nextchar[2], '\0');
	}

	it("reads in files") {
		m_stream s;
		FILE *f = fopen("assets/stream_1", "r");
		m_stream_init_file(&s, f);

		asserteq(s.nextchar[0], 'a');
		asserteq(s.nextchar[1], 'f');
		asserteq(s.nextchar[2], '\0');

		fclose(f);
	}
}
