#include "test.h"

struct test _local_test;

struct test _global_test = {
	.description = "All Tests",
	.done = 0,
	.ntests = 0,
	.npassed = 0,
	.file = NULL,
	.line = 0,
	.starttime = -1
};

int _test_exit_status = 0;
int _test_indents = -1;

static int nesting = 0;

void _local_test_run(char *name, void (*fun)())
{
	_test_indents += 1;

	nesting = _test_indents;

	_TEST_INDENT(indent, _test_indents);

	_local_test.done = 0;
	_local_test.ntests = 0;
	_local_test.npassed = 0;
	_local_test.starttime = 0;

	_local_test.ntests = 0;
	_local_test.npassed = 0;
	printf("\n" TEST_COLOR_BOLD "%sRunning %s:" TEST_COLOR_RESET "\n",
		indent, name);

	clock_t starttime = clock();
	if (_global_test.starttime == -1)
		_global_test.starttime = starttime;

	(*fun)();
	int msec = (clock() - starttime) * 1000 / CLOCKS_PER_SEC;
	_local_test_done();

	if (nesting == _test_indents)
	{
		fprintf(stderr,
			TEST_COLOR_BOLD "%s%s: Passed %i/%i tests (%dms)."
			TEST_COLOR_RESET "\n",
			indent, name, _local_test.npassed,
			_local_test.ntests, msec);

		_global_test.ntests += _local_test.ntests;
		_global_test.npassed += _local_test.npassed;
	}

	_test_indents -= 1;
}

void _local_test_done()
{
	if (_local_test.done)
		return;

	_local_test.done = 1;
	_local_test.ntests += 1;
	_local_test.npassed += 1;

	int msec = (clock() - _local_test.starttime) * 1000 / CLOCKS_PER_SEC;

	_TEST_INDENT(indent, _test_indents);

	fprintf(stderr,
		TEST_COLOR_BOLD TEST_COLOR_SUCCESS "%s✓ "
		TEST_COLOR_RESET TEST_COLOR_SUCCESS "Success: "
		TEST_COLOR_RESET TEST_COLOR_DESC "%s "
		TEST_COLOR_RESET "(%dms)\n",
		indent, _local_test.description, msec);
}

int test_suite_done()
{
	int msec = (clock() - _global_test.starttime) * 1000 / CLOCKS_PER_SEC;

	fprintf(stderr,
		"\n" TEST_COLOR_BOLD "Total: Passed %i/%i tests (%dms)."
		TEST_COLOR_RESET "\n",
		_global_test.npassed, _global_test.ntests, msec);

	return _test_exit_status;
}
