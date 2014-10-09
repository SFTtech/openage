#include "testing.h"

#include <cstdio>
#include <cstring>
#include <fnmatch.h>
#include <string>
#include <unordered_map>

#include "../log.h"
#include "../util/error.h"

using namespace openage;

namespace testing {

struct test {
	std::string description;
	bool interactive;
	test_function_t fp;
	bool result;
};

std::unordered_map<std::string, test> tests;

void register_test(std::string name, std::string description, bool interactive, test_function_t fp) {
	if (tests.find(name) != tests.end()) {
		throw util::Error("test %s already exists", name.c_str());
	}
	tests[name] = test{description, interactive, fp, false};
}

bool run_tests(const char *expr, bool no_interactive, int argc, char **argv) {
	int interactive_count = 0;
	int executed_count    = 0;
	int success_count     = 0;
	int failed_count      = 0;

	//run the tests
	for (auto &test : tests) {
		const char *test_name = test.first.c_str();

		//do we wanna execute it?
		if (fnmatch(expr, test_name, 0) != 0) {
			continue;
		}

		if (test.second.interactive) {
			interactive_count += 1;
			if (no_interactive) {
				continue;
			}
		}

		//run the test
		log::msg("running test %s", test_name);
		bool test_result = test.second.fp(argc, argv);
		test.second.result = test_result;

		if (test_result == true) {
			log::msg("test succeeded: %s", test_name);
			success_count += 1;
		}
		else {
			log::err("test failed:    %s", test_name);
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

	if (executed_count > 1 and failed_count > 0) {
		log::err("testing was not successful!");
		return false;
	}
	else if (executed_count > 0) {
		return true;
	} else if (interactive_count > 0) {
		log::err("all tests that match '%s' are interactive", expr);
		return false;
	} else {
		log::err("no tests match '%s'", expr);
		return false;
	}
}

void list_tests() {
	if (tests.empty()) {
		printf("no tests are available\n");
		return;
	}

	for (auto &test : tests) {
		const char *is_interactive = test.second.interactive ? "\n\t[interactive]" : "";
		printf("%s\n\t%s%s\n\n", test.first.c_str(), test.second.description.c_str(), is_interactive);
	}
}

} //namespace testing
