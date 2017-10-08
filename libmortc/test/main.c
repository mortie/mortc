#include "test.h"

void test_stream();

int main()
{
	run(test_stream);

	return test_exit_status;
}
