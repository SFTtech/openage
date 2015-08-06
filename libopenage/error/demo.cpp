// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include <vector>
#include <string>

#include "backtrace.h"
#include "error.h"

namespace openage {
namespace error {


// anonymous namespace to prevent linkage for exception demo helper functions.
namespace {


void foo() {
	throw Error(MSG(err) << "what an exceptional line of code!", true);
}


void bar(int i) {
	if (i % 5 == 3) {
		try {
			foo();
		} catch (...) {
			// Note: pokemon exception handling is generally discouraged.
			// This serves as a demo how the Error constructor nevertheless
			// manages to correctly capture the cause exception.
			// Even if you don't want to access the object, `catch (Error &)`.

			throw Error(MSG(crit).fmt("exception in foo. i=%d", i));
		}
	} else {
		bar(i + 1);
	}
}


} // anonymous namespace


void demo() {
	try {
		bar(0);
	} catch (Error &exc) {
		if (exc.backtrace) {
			exc.trim_backtrace();
		}

		log::log(MSG(info) <<
			"exception_demo: captured the following exception:" << std::endl <<
			exc << std::endl <<
			"exception_demo: end of exception");
	}
}


}} // openage::error
