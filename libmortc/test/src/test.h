#ifndef TEST_H
#define TEST_H

#include <string.h>
#include <stdio.h>
#include <time.h>

#define TEST_COLOR_SUCCESS "\033[32m"
#define TEST_COLOR_FAIL    "\033[31m"
#define TEST_COLOR_DESC    "\033[1m\033[33m"
#define TEST_COLOR_BOLD    "\033[1m"
#define TEST_COLOR_RESET   "\033[0m"

/*
 * Example:
 *
 * void test_something() {
 *     it("does a") {
 *         asserteq(1, 1);
 *     }
 *
 *     it("does b") {
 *         assert(5 == 5);
 *     }
 * }
 *
 * int main() {
 *     run(test_something);
 * }
 */

extern struct test {
	char description[256];
	int done;
	int ntests;
	int npassed;

	char *file;
	int line;

	clock_t starttime;
} _test;

extern int test_exit_status;

void _test_done();

void _test_run(char *name, void (*fun)());

#define it(desc) \
	_test_done(); \
	_test.file = __FILE__; \
	_test.line = __LINE__; \
	_test.starttime = clock(); \
	strncpy(_test.description, desc, sizeof(_test.description)); \
	_test.done = 0;

#define run(name) _test_run(#name, &name)

#define fail(...) \
	do { \
		if (_test.done) \
			break; \
		fprintf(stderr, \
			TEST_COLOR_BOLD TEST_COLOR_FAIL "✕ " \
			TEST_COLOR_RESET TEST_COLOR_FAIL "Failed: " \
			TEST_COLOR_RESET TEST_COLOR_DESC "%s: " \
			TEST_COLOR_RESET, _test.description); \
		fprintf(stderr, __VA_ARGS__); \
		fprintf(stderr, "\n    at %s:%i\n", __FILE__, __LINE__); \
		_test.done = 1; \
		_test.ntests += 1; \
		test_exit_status = 1; \
	} while (0)

#define assert(x) \
	do { \
		if (!(x)) { \
			fail(#x); \
		} \
	} while (0)

#define assertstr(a, b) \
	do { \
		char *str = (a); \
		if (str == NULL) {\
			fail("Expected " #a " to equal " #b ", got NULL"); \
		} else if (strcmp(str, (b)) != 0) { \
			fail("Expected " #a " to equal " #b \
				", got \"%s\"", str); \
		} \
	} while (0)

#define assertint(a, b) \
	do { \
		int num = (a); \
		if (num != (b)) { \
			fail("Expected " #a " to equal " #b \
				", got %i", num); \
		} \
	} while (0)

#endif
