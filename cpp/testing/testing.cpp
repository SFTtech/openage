// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "testing.h"

#include <string>
#include <vector>
#include <map>

#include "testlist.h"
#include "../log/log.h"

using namespace openage;

namespace testing {

bool run_test(const std::string &name) {
	auto test = tests.find(name);
	if (test == tests.end()) {
		log::log(MSG(err) << "No such test: " << name);
		return false;
	}

	try {
		test->second();
		return true;
	} catch(... /* gotta catch em all TODO change when new exception system is finished */) {
		log::log(MSG(err) << "Test failed: " << name);
		return false;
	}
}

bool run_demo(const std::string &name, int argc, char **argv) {
	auto demo = demos.find(name);
	if (demo == demos.end()) {
		log::log(MSG(err) << "No such demo: " << name);
		return false;
	}

	try {
		demo->second(argc, argv);
		return true;
	} catch (... /* gotta catch em all */) {
		log::log(MSG(err) << "Demo failed: " << name);
		return false;
	}
}

void list_tests() {
	if (tests.empty()) {
		std::cout << "No tests are available" << std::endl;
	} else {
		for (auto &test : tests) {
			// TODO load description from tests_cpp asset
			std::cout <<
				"[test] " << test.first << ": " <<
				"(no description loaded)" <<
				std::endl;
		}
	}

	if (demos.empty()) {
		std::cout << "no demos are available" << std::endl;
	} else {
		for (auto &demo : demos) {
			std::cout <<
				"[demo] " << demo.first << ": " <<
				"(no description loaded)" <<
				std::endl;
		}
	}
}

} //namespace testing
