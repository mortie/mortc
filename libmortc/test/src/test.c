#include "test.h"

#define COLOR_SUCCESS "\033[32m"
#define COLOR_FAIL    "\033[31m"
#define COLOR_DESC    "\033[1m\033[33m"
#define COLOR_BOLD    "\033[1m"
#define COLOR_RESET   "\033[0m"

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
	printf("\n" COLOR_BOLD "Running %s:" COLOR_RESET "\n\n", name);
	int starttime = clock();
	(*fun)();
	int msec = (clock() - starttime) * 1000 / CLOCKS_PER_SEC;
	_test_done();
	printf("\n" COLOR_BOLD "Passed %i/%i tests (%dms)." COLOR_RESET "\n\n",
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
	printf(
		COLOR_BOLD COLOR_SUCCESS "✓ "
		COLOR_RESET COLOR_SUCCESS "Success: "
		COLOR_RESET COLOR_DESC "%s "
		COLOR_RESET "(%dms)",
		_test.description, msec);
	printf("\n");
}

void _test_fail(char *comment)
{
	if (_test.done)
		return;

	_test.done = 1;
	_test.ntests += 1;

	printf(
		COLOR_BOLD COLOR_FAIL "✕ "
		COLOR_RESET COLOR_FAIL "Failed: "
		COLOR_RESET COLOR_DESC "%s: "
		COLOR_RESET "%s\n", _test.description, comment);
	printf("    at %s:%i\n", _test.file, _test.line);

	test_exit_status = 1;
}
