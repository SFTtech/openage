// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "exctranslate.h"

#include <string>

#include "../util/timing.h"
#include "../error/error.h"
#include "../util/compiler.h"
#include "../util/thread_id.h"
#include "../log/level.h"

#include "pyexception.h"

namespace openage {
namespace pyinterface {


// python exception translation function pointers
void (*raise_cpp_error)(Error *) = nullptr;
void (*raise_cpp_pyexception)(PyException *) = nullptr;

void (*describe_py_exception)(PyException *) = nullptr;
bool (*check_for_py_exception)() = nullptr;


void set_exc_translation_funcs(
	void (*raise_cpp_error_impl)(Error *),
	void (*raise_cpp_pyexception_impl)(PyException *),
	bool (*check_for_py_exception_impl)(),
	void (*describe_py_exception_impl)(PyException *)) {

	raise_cpp_error = raise_cpp_error_impl;
	raise_cpp_pyexception = raise_cpp_pyexception_impl;
	check_for_py_exception = check_for_py_exception_impl;
	describe_py_exception = describe_py_exception_impl;
}


void translate_exc_cpp_to_py() {

	try {
		throw;

	} catch (PyException &exc) {

		if (unlikely(raise_cpp_pyexception == nullptr)) {
			throw Error(MSG(err) <<
				"raise_pyexception_in_py is uninitialized; "
				"can't translate C++ exception to Python exception.",
				false, false);
		}

		raise_cpp_pyexception(&exc);

	} catch (Error &exc) {

		if (unlikely(raise_cpp_error == nullptr)) {
			throw Error(MSG(err) <<
				"raise_error_in_py is uninitialized; "
				"can't translate C++ exception to Python exception.",
				false, false);
		}

		raise_cpp_error(&exc);
	}

	/*
	 * all other, non-openage::Error exceptions are unexpected;
	 * they don't contain any useful stack trace information, so the
	 * safest course of action is not to catch them.
	 * That way, terminate() is called, where we can analyze the stack
	 * trace and debug the issue in gdb.
	 */
}


void translate_exc_py_to_cpp() {
	if (unlikely(describe_py_exception == nullptr)) {
		throw Error(MSG(err) <<
			"describe_py_exception is uninitialized; "
			"can't check for and translate Python exception to C++ exception.");
	}

	if (likely(not check_for_py_exception())) {
		// no exception has occurred.
		return;
	}

	PyException pyex;
	describe_py_exception(&pyex);

	// recurse to throw a possible cause.
	try {
		translate_exc_py_to_cpp();
	} catch (...) {
		pyex.store_cause();
	}

	// throw exception without cause.
	throw pyex;
}


void init_exc_message(log::message *msg, const std::string &filename, unsigned int lineno, const std::string &functionname) noexcept {
	try {
		msg->init_with_metadata_copy(filename, functionname);
	} catch (...) {
		// we cannot afford to raise an exception from this function; this is part of the exception translation code.
	}

	msg->lvl = log::lvl::err;
	msg->lineno = lineno;
}


}} // openage::pyinterface
