#include "test.h"

struct test _test;

int test_exit_status = 0;

void _test_run(char *name, void (*fun)())
{
	_test.done = 1;
	_test.ntests = 0;
	_test.npassed = 0;
	_test.starttime = 0;

	_test.ntests = 0;
	_test.npassed = 0;
	printf("\n" TEST_COLOR_BOLD "Running %s:" TEST_COLOR_RESET "\n\n", name);

	int starttime = clock();
	(*fun)();
	int msec = (clock() - starttime) * 1000 / CLOCKS_PER_SEC;
	_test_done();

	fprintf(stderr,
		"\n" TEST_COLOR_BOLD "Passed %i/%i tests (%dms)."
		TEST_COLOR_RESET "\n\n",
		_test.npassed, _test.ntests, msec);
}

void _test_done()
{
	if (_test.done)
		return;

	_test.done = 1;
	_test.ntests += 1;
	_test.npassed += 1;

	int msec = (clock() - _test.starttime) * 1000 / CLOCKS_PER_SEC; \
	fprintf(stderr,
		TEST_COLOR_BOLD TEST_COLOR_SUCCESS "✓ "
		TEST_COLOR_RESET TEST_COLOR_SUCCESS "Success: "
		TEST_COLOR_RESET TEST_COLOR_DESC "%s "
		TEST_COLOR_RESET "(%dms)\n",
		_test.description, msec);
}
