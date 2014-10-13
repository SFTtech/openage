#include "testing.h"

#include <string>
#include <vector>
#include <map>

#include "testlist.h"
#include "../log.h"
#include "../util/error.h"

using namespace openage;

namespace testing {

bool run_test(std::string name) {
	auto test = tests.find(name);
	if (test == tests.end()) {
		log::err("no such test: %s", name.c_str());
		return false;
	}

	try {
		test->second();
		return true;
	}
	catch(... /* gotta catch em all TODO change when new exception system is finished */) {
		log::err("test failed: %s", name.c_str());
		return false;
	}
}

void list_tests() {
	if (tests.empty()) {
		printf("no tests are available\n");
		return;
	}

	for (auto &test : tests) {
		// TODO load description from tests_cpp asset
		printf("%s: %s\n", test.first.c_str(), "(no description loaded)");
	}
}

} //namespace testing
