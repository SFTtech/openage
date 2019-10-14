// Copyright 2015-2019 the openage authors. See copying.md for legal info.

#include "setup.h"

#include <map>
#include <mutex>
#include <set>
#include <utility>

#include "../error/error.h"
#include "../util/compiler.h"

namespace openage::pyinterface {


// no need for these global objects to have any linkage.
namespace {


struct state {
	// secures the state object
	std::mutex lock;

	// holds the checker function for each component
	std::map<void *, std::function<bool ()>> map;

	// holds the components that have been destroyed so far
	std::set<void *> destroyed_components;

	// set to true once the check has run for the first time.
	bool check_performed;

	// returns the global singleton object
	static state &get() {
		static state val{{}, {}, {}, false};
		return val;
	}
};


} // anonymous namespace


void add_py_if_component(void *thisptr, std::function<bool ()> checker) {
	state &checkers = state::get();

	std::unique_lock<std::mutex> lock{checkers.lock};

	// enforce that the object has an associated symbol name.
	if (unlikely(not util::is_symbol(thisptr))) {
		throw Error(
			MSG(err) <<
			"Can't instantiate py interface component as non-global object. " <<
			"Attempted instantiation at " <<
			util::symbol_name(thisptr) << ".",
			true);
	}

	checkers.map[thisptr] = std::move(checker);
}


void destroy_py_if_component(void *thisptr) {
	state &checkers = state::get();

	std::unique_lock<std::mutex> lock{checkers.lock};

	checkers.destroyed_components.insert(thisptr);
	checkers.map.erase(thisptr);
}


void check() {
	state &checkers = state::get();

	std::unique_lock<std::mutex> lock{checkers.lock};

	if (checkers.check_performed) {
		throw Error(MSG(err) <<
			"py interface initialization check has already been performed. "
			"fix your init code.");
	}

	checkers.check_performed = true;

	bool error_found = false;
	auto msg = MSG(err);
	msg << "Fix your init code: ";

	if (not checkers.destroyed_components.empty()) {
		msg << std::endl <<
			"Py interface components have been de-initialized before "
			"initialization had even been completed:";

		for (void *thisptr : checkers.destroyed_components) {
			msg << std::endl << "\t" << util::symbol_name(thisptr);
		}

		error_found = true;
	}

	checkers.destroyed_components.clear();

	bool first = true;
	for (auto &checker : checkers.map) {
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

	checkers.map.clear();

	if (error_found) {
		throw Error(msg);
	}
}


} // openage::pyinterface
