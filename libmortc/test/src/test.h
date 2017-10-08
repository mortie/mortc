#ifndef TEST_H
#define TEST_H

#include <string.h>
#include <stdio.h>
#include <time.h>

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
 *
 *     done();
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

void _test_fail(char *comment);
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

#define fail(x) \
	do { \
		_test.line = __LINE__; \
		_test_fail(x); \
	} while (0)

#define assert(x) \
	do { \
		if (!(x)) { \
			fail(#x); \
		} \
	} while (0)

#define assertstr(a, b) \
	do { \
		if (strcmp((a), (b)) != 0) { \
			fail("Expected " #a " to equal " #b); \
		} \
	} while (0)

#define asserteq(a, b) \
	do { \
		if ((a) != (b)) { \
			fail("Expected " #a " to equal " #b); \
		} \
	} while (0)


#endif
