// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "init.h"

#include "../testing/testing.h"

namespace openage {
namespace util {
namespace tests {

// those test variables need no linkage.
namespace {
	int testval;

	OnInit init_testval([]() {
		testval = 5;
	});
}

void init() {
	TESTEQUALS(testval, 5);

	int i = 6;

	TESTEQUALS(i, 6);

	{
		OnInit init_i([&]() {
			i = 7;
		});

		OnDeInit deinit_i([&]() {
			i = 8;
		});

		TESTEQUALS(i, 7);
	}

	TESTEQUALS(i, 8);
}

}}}// openage::util::tests
