#include "testing.h"

#include <string.h>
#include <vector>

#include "../engine/log.h"
#include "../engine/util/error.h"


using namespace engine;

namespace test {

std::map<const char *, bool (*)()> tests;


void register_test(const char *name, bool (*test_function)()) {
	test::tests[name] = test_function;
}


bool run_test(const char *name) {
	for (auto &test : tests) {
		const char *test_name = test.first;

		if (0 == strcmp(test_name, name)) {
			bool (*test_function)() = test.second;
			return test_function();
		}
	}

	throw Error("test '%s' not found.", name);
}

void list_tests() {
	size_t test_count = tests.size();

	if (test_count > 0) {
		log::msg("available tests: %lu", test_count);
		log::msg("test list:");

		//print all available test names
		size_t i = 0;
		for (auto &test_name : tests) {
			log::msg("test %03lu: '%s'", i, test_name.first);
			i += 1;
		}
	}
	else {
		log::msg("No tests available.");
	}
}


void run_tests(std::vector<const char *> names) {

	//run all tests in the list
	for (auto &test_name : names) {
		try {
			bool result = run_test(test_name);

			if (not result) {
				log::err("test failed: %s", test_name);
			}
			else {
				log::msg("test succeeded: %s", test_name);
			}
		} catch (Error e) {
			log::err("test aborted: %s: %s", test_name, e.str());
		}
	}
}

//place all tests to be active in here.
void test_activation() {
	register_test("metatest", &test_metatest);
}

} //namespace test
