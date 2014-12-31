// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#include "testing.h"

#include <string>
#include <vector>
#include <map>

#include "testlist.h"
#include "../log.h"
#include "../util/error.h"

using namespace openage;

namespace testing {

bool run_test(const std::string &name) {
	auto test = tests.find(name);
	if (test == tests.end()) {
		log::err("no such test: %s", name.c_str());
		return false;
	}

	try {
		test->second();
		return true;
	} catch(... /* gotta catch em all TODO change when new exception system is finished */) {
		log::err("test failed: %s", name.c_str());
		return false;
	}
}

bool run_demo(const std::string &name, int argc, char **argv) {
	auto demo = demos.find(name);
	if (demo == demos.end()) {
		log::err("no such demo: %s", name.c_str());
		return false;
	}

	try {
		demo->second(argc, argv);
		return true;
	} catch (... /* gotta catch em all */) {
		log::err("demo failed: %s", name.c_str());
		return false;
	}
}

void list_tests() {
	if (tests.empty()) {
		printf("no tests are available\n");
	} else {
		for (auto &test : tests) {
			// TODO load description from tests_cpp asset
			printf("[test] %s: %s\n", test.first.c_str(), "(no description loaded)");
		}
	}

	if (demos.empty()) {
		printf("no demos are available\n");
	} else {
		for (auto &demo : demos) {
			printf("[demo] %s: %s\n", demo.first.c_str(), "(no description loaded)");
		}
	}
}

} //namespace testing
