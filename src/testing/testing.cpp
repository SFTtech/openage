#include "testing.h"

#include <string.h>
#include <stdio.h>
#include <vector>
#include <map>

#include <fnmatch.h>

#include "../engine/log.h"
#include "../engine/util/error.h"

using namespace engine;

namespace testing {

struct test {
	const char *description;
	bool interactive;
	test_function_t fp;
};

std::map<const char *, test> tests;

void register_test(const char *name, const char *description, bool interactive, test_function_t fp) {
	if (tests.count(name) > 0) {
		throw Error("test %s already exists", name);
	}
	tests[name] = test{description, interactive, fp};
}

void run_tests(const char *expr, bool no_interactive, int argc, char **argv) {
	int interactive_count = 0;
	int total_count = 0;
	int completed = 0;

	for (auto &test : tests) {
		if (fnmatch(expr, test.first, 0) != 0) {
			continue;
		}

		total_count += 1;
		if (test.second.interactive) {
			interactive_count += 1;
			if (no_interactive) {
				continue;
			}
		}

		log::msg("running test %s", test.first);

		if (!test.second.fp(argc, argv)) {
			throw Error("test %s failed", test.first);
		}

		completed += 1;
	}

	if (completed > 0) {
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
