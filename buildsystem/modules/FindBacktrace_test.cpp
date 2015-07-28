// Copyright 2015-2015 the openage authors. See copying.md for legal info.

/**
 * Backtrace tester: Tests whether <backtrace.h> is available.
 *
 * Needs to be compiled with -lbacktrace.
 */


#include <iostream>

#include <backtrace.h>


int callback(void * /* unused */, uintptr_t pc) {
	std::cout << (void *) pc << std::endl;
	return 0;
}


int main() {
	struct backtrace_state *state = nullptr;

	state = backtrace_create_state(nullptr, false, nullptr, nullptr);

	backtrace_simple(state, 0, callback, nullptr, nullptr);
}
