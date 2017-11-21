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

#define _TEST_INDENT(var, n) \
	char var[(n * 2) + 1]; \
	memset(var, ' ', sizeof(var)); \
	var[sizeof(var) - 1] = '\0';

struct test {
	char description[256];
	int done;
	int ntests;
	int npassed;

	char *file;
	int line;

	clock_t starttime;
};

extern struct test _local_test;
extern struct test _global_test;

extern int _test_exit_status;
extern int _test_indents;

void _local_test_done();

void _local_test_run(char *name, void (*fun)());

int test_suite_done();

#define it(desc) \
	_local_test_done(); \
	_local_test.file = __FILE__; \
	_local_test.line = __LINE__; \
	_local_test.starttime = clock(); \
	strncpy(_local_test.description, desc, sizeof(_local_test.description)); \
	_local_test.done = 0;

#define run(name) _local_test_run(#name, &name)

#define fail(...) \
	do { \
		if (_local_test.done) break; \
		_TEST_INDENT(indent, _test_indents); \
		fprintf(stderr, \
			TEST_COLOR_BOLD TEST_COLOR_FAIL "%s✕ " \
			TEST_COLOR_RESET TEST_COLOR_FAIL "Failed: " \
			TEST_COLOR_RESET TEST_COLOR_DESC "%s:\n%s    " \
			TEST_COLOR_RESET, indent, _local_test.description, indent); \
		fprintf(stderr, __VA_ARGS__); \
		fprintf(stderr, "\n%s    at %s:%i\n", indent, __FILE__, __LINE__); \
		_local_test.done = 1; \
		_local_test.ntests += 1; \
		_test_exit_status = 1; \
	} while (0)

#define assert(x) \
	do { \
		if (!(x)) { \
			fail(#x); \
		} \
	} while (0)

#define assertstr(a, b) \
	do { \
		if (_local_test.done) break; \
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
		if (_local_test.done) break; \
		int num = (a); \
		if (num != (b)) { \
			fail("Expected " #a " to equal " #b \
				", got %i", num); \
		} \
	} while (0)

#define assertdbl(a, b) \
	do { \
		if (_local_test.done) break; \
		double num = (a); \
		if (num != (b)) { \
			fail("Expected " #a " to equal " #b \
				", got %f", num); \
		} \
	} while (0)

#endif
