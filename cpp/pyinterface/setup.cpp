// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "setup.h"

#include <mutex>
#include <set>
#include <map>

#include "../error/error.h"
#include "../util/compiler.h"

namespace openage {
namespace pyinterface {


// no need for these global objects to have any linkage.
namespace {


/**
 * Secures access to all of these globals.
 */
auto &checker_lock() {
	static std::mutex val;
	return val;
}


/**
 * Holds all checker functions.
 */
auto &checker_map() {
	static std::map<void *, std::function<bool ()>> val;
	return val;
}


/**
 * Holds all components that have been destroyed so far.
 */
auto &destroyed_components() {
	static std::set<void *> val;
	return val;
}


/**
 * Set to 'true' once the check has run for the first time.
 */
auto &check_performed() {
	static bool val = false;
	return val;
}

} // anonymous namespace


void add_py_if_component(void *thisptr, std::function<bool ()> checker) {
	std::unique_lock<std::mutex> lock{checker_lock()};

	// enforce that the object has an associated symbol name.
	if (unlikely(not util::is_symbol(thisptr))) {
		throw Error(
			MSG(err) <<
			"Can't instantiate py interface component as non-global object. " <<
			"Attempted instantiation at " <<
			util::symbol_name(thisptr) << ".",
			true);
	}

	checker_map()[thisptr] = checker;
}


void destroy_py_if_component(void *thisptr) {
	std::unique_lock<std::mutex> lock{checker_lock()};

	destroyed_components().insert(thisptr);
	checker_map().erase(thisptr);
}


void check() {
	std::unique_lock<std::mutex> lock{checker_lock()};

	if (check_performed()) {
		throw Error(MSG(err) <<
			"py interface initialization check has already been performed. "
			"fix your init code.");
	}

	check_performed() = true;

	bool error_found = false;
	auto msg = MSG(err);
	msg << "Fix your init code: ";

	if (not destroyed_components().empty()) {
		msg << std::endl <<
			"Py interface components have been de-initialized before "
			"initialization had even been completed:";

		for (void *thisptr : destroyed_components()) {
			msg << std::endl << "\t" << util::symbol_name(thisptr);
		}

		error_found = true;
	}

	destroyed_components().clear();

	bool first = true;
	for (auto &checker : checker_map()) {
		if (not checker.second()) {
			if (first) {
				msg << std::endl <<
					"The following components have not been initialized:";

				first = false;
				error_found = true;
			}

			msg << std::endl << "\t" << util::symbol_name(checker.first);
		}
	}

	checker_map().clear();

	if (error_found) {
		throw Error(msg);
	}
}


}} // openage::pyinterface
