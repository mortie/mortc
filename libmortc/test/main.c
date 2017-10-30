#include "test.h"

void test_stream();
void test_parser();

int main()
{
	run(test_stream);
	run(test_parser);

	return test_exit_status;
}
