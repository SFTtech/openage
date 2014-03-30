#include "testing.h"

#include <fnmatch.h>
#include <map>
#include <stdio.h>
#include <string.h>

#include "../engine/log.h"
#include "../engine/util/error.h"

using namespace engine;

namespace testing {

struct test {
	const char *description;
	bool interactive;
	test_function_t fp;
	bool result;
};

std::map<const char *, test> tests;

void register_test(const char *name, const char *description, bool interactive, test_function_t fp) {
	if (tests.count(name) > 0) {
		throw Error("test %s already exists", name);
	}
	tests[name] = test{description, interactive, fp, false};
}

void run_tests(const char *expr, bool no_interactive, int argc, char **argv) {
	int interactive_count = 0;
	int executed_count    = 0;
	int success_count     = 0;
	int failed_count      = 0;

	//run the tests
	for (auto &test : tests) {
		//do we wanna execute it?
		if (fnmatch(expr, test.first, 0) != 0) {
			continue;
		}

		if (test.second.interactive) {
			interactive_count += 1;
			if (no_interactive) {
				continue;
			}
		}

		//run the test
		log::msg("running test %s", test.first);
		bool test_result = test.second.fp(argc, argv);
		test.second.result = test_result;

		if (test_result == true) {
			log::msg("test succeeded: %s", test.first);
			success_count += 1;
		}
		else {
			log::err("test failed:    %s", test.first);
			failed_count += 1;
		}

		executed_count += 1;
	}

	if (executed_count > 1) {
		log::msg("== testing summary ==");
		log::msg("tests executed:  %d", executed_count);
		log::msg("tests succeeded: %d", success_count);
		log::msg("tests failed:    %d", failed_count);
	}

	//some test failed
	if (failed_count > 0) {
		throw Error("testing was not successful (but still the future)!");
	}

	if (executed_count > 0) {
		return;
	} else if (interactive_count > 0) {
		throw Error("all tests that match '%s' are interactive", expr);
	} else {
		throw Error("no tests match '%s'", expr);
	}
}

void list_tests() {
	if (tests.empty()) {
		printf("no tests are available\n");
		return;
	}

	for (auto &test : tests) {
		printf("%s\n\t%s%s\n\n", test.first, test.second.description, test.second.interactive ? "\n\t[interactive]" : "");
	}
}

} //namespace testing
