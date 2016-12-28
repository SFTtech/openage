// Copyright 2015-2016 the openage authors. See copying.md for legal info.

/*
 * This file holds handlers for std::terminate and SIGSEGV.
 *
 * The handlers print stack trace and (for terminate) exception information,
 * before allowing the program to exit.
 *
 * The handlers are installed when loading the library, and uninstalled
 * when unloading it.
 */

#include <exception>
#include <iostream>
#include <typeinfo>
#include <cstring>

#include <unistd.h>

#include "../util/signal.h"
#include "../util/init.h"
#include "../util/language.h"

#include "error.h"
#include "stackanalyzer.h"

namespace openage {
namespace error {


[[noreturn]] void terminate_handler() noexcept;
void sigsegv_handler(int /* unused */);
void exit_handler();

// The global state has internal linkage only.
namespace {

bool exit_ok;

std::terminate_handler old_terminate_handler;
sighandler_t old_sigsegv_handler;

util::OnInit install_handlers([]() {
	old_sigsegv_handler = signal(SIGSEGV, sigsegv_handler);
	old_terminate_handler = std::set_terminate(terminate_handler);
	exit_ok = true;
	std::atexit(exit_handler);
});


util::OnDeInit restore_handlers([]() {
	std::set_terminate(old_terminate_handler);
	signal(SIGSEGV, old_sigsegv_handler);
});


} // anonymous namespace


[[noreturn]] void terminate_handler() noexcept {
	// immediately unset this handler, to avoid endless recursions if
	// terminate() is accidentially triggered from here.
	std::set_terminate(old_terminate_handler);

	std::cout << "\n\x1b[31;1mFATAL: terminate has been called\x1b[m" << std::endl;

	if (std::exception_ptr e_ptr = std::current_exception()) {
		std::cout << "\n\x1b[33muncaught exception\x1b[m\n" << std::endl;

		try {
			std::rethrow_exception(e_ptr);
		} catch (Error &exc) {
			std::cout << exc << std::endl;
		} catch (std::exception &exc) {
			std::cout <<
				"std::exception of type " <<
				util::demangle(typeid(exc).name()) <<
				": " << exc.what() << std::endl;
		} catch (...) {
			std::cout << "non-standard exception object" << std::endl;
		}
	}

	std::cout << "\n\x1b[33mcurrent stack\x1b[m\n" << std::endl;

	StackAnalyzer backtrace;
	backtrace.analyze();
	std::cout << backtrace << std::endl;

	// call the original handler, to enable debugger functionality,
	// and maybe print some additional useful info that we forgot about.
	std::cout << "\x1b[33mstandard terminate handler\x1b[m\n" << std::endl;
	std::terminate();
}


void sigsegv_handler(int /* unused */) {
	// In theory, this handler may only call async-signal-safe functions,
	// such as write().
	const char *message = "\n\x1b[31;1mSIGSEGV\x1b[m\n";
	util::ignore_result(write(1, message, strlen(message)));

	// however, everything is broken anyways. can't hurt to try to print
	// more useful info. fuck the police! wheeee!
	std::terminate();
}


void exit_handler() {
	// This handler is registered to run atexit().
	// It is used to catch calls to exit() that occur somewhere inside
	// the running game (while exit_ok == false).
	// exit() should never be invoked directly while the game is running,
	// but some libraries such as libepoxy might do it anyway.
	// The actual proper way of exiting the running game is via throwing
	// an exception or similar action.

	if (exit_ok) { return; }

	std::cout << "\x1b[31;1mexit() was called in an illegal place\x1b[m\n" << std::endl;
}


void set_exit_ok(bool value) {
	exit_ok = value;
}


}} // openage::error
