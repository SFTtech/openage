// Copyright 2013-2017 the openage authors. See copying.md for legal info.

#pragma once

// pxd: from libcpp cimport bool
#include <iostream>
#include <memory>
#include <stdexcept>
// pxd: from libcpp.string cimport string
#include <string>

#include "../util/compiler.h"
// pxd: from libopenage.log.message cimport message
#include "../log/message.h"

// pxd: from libopenage.error.backtrace cimport Backtrace
namespace openage {
namespace error {
// forward-declaration to avoid the header include.
class Backtrace;
}}

namespace openage {
namespace pyinterface {
// forward-declaration for use in the 'friend' declaration below.
class PyException;
}}


namespace openage {
namespace error {

/**
 * Openage base exception type; the constructor usage is analogous to
 * log::log().
 *
 * pxd:
 * void debug_break_on_error "::openage::error::Error::debug_break_on_create"(bool state) except +
 *
 * cppclass Error:
 *     message msg
 *
 *     string type_name() except +
 *     const char *what() except +
 *     void rethrow_cause() except +
 *     void trim_backtrace() except +
 *
 *     Backtrace *backtrace
 */
class Error : public std::runtime_error {
public:
	/**
	 * @param msg
	 *     As with log::log()
	 * @param generate_backtrace
	 *     If true, some platform-specific code is run to collect
	 *     traceback information (e.g.: backtrace (3))
	 *     (default true).
	 *     The performance impacts should be not too bad, as only
	 *     program counter pointers are collected.
	 * @param store_cause
	 *     If true, a pointer to the causing exception is
	 *     collected and stored (default true).
	 */
	Error(const log::message &msg, bool generate_backtrace=true, bool store_cause=true);


	/**
	 * Stores a pointer to the currently-handled exception in this->cause.
	 */
	void store_cause();


	/**
	 * Calls this->backtrace->trim_to_current_stack_frame(),
	 * if this->backtrace is not nullptr.
	 *
	 * Designed to be used in catch clauses, to strip away all those
	 * unneeded symbols from program init upwards.
	 *
	 * Automatically called for cause exceptions, and when storing cause exceptions,
	 * and by the to_py converter.
	 */
	void trim_backtrace();


	/**
	 * The error message.
	 */
	log::message msg;


	/**
	 * The (optional) backtrace.
	 */
	std::shared_ptr<Backtrace> backtrace;


	/**
	 * Re-throws the exception cause, if the exception has one.
	 * Otherwise, does nothing.
	 *
	 * Use this when handling the exception, to handle the cause.
	 */
	void rethrow_cause() const;


	/**
	 * The type name of the exception (for pretty-printing in case
	 * the exception hierarchy is used).
	 * Uses typeid internally.
	 */
	virtual std::string type_name() const;


	/**
	 * Returns the message's content.
	 */
	const char *what() const noexcept override;

	/**
	 * Turn on debug breaks in the constructor
	 */
	static void debug_break_on_create(bool state);

private:
	friend pyinterface::PyException;


	/**
	 * Constructs an empty error.
	 * For use by our friend, pyinterface::PyException.
	 */
	Error();


	/**
	 * Re-throw this with rethrow_cause().
	 */
	std::exception_ptr cause;
};


std::ostream &operator <<(std::ostream &os, const Error &e);


[[deprecated("add message to the ENSURE before pushing, please")]]
inline std::string no_ensuring_message()
{
	return std::string{};
}

// ENSURE(condition, errormessage << variable << etcetc)
#define ENSURE(...) do if (unlikely(not OPENAGE_ENS_FIRST(__VA_ARGS__))) throw ::openage::error::Error(MSG(err) OPENAGE_ENS_REST(__VA_ARGS__)); while (0)

/*
 *  expands to the first argument
 */
#define OPENAGE_ENS_FIRST(...) OPENAGE_ENS_FIRST_HELPER(__VA_ARGS__, throwaway)
#define OPENAGE_ENS_FIRST_HELPER(first, ...) (first)

/*
 * Standard alternative to GCC's ##__VA_ARGS__ trick (Richard Hansen)
 * http://stackoverflow.com/a/11172679/4742108
 *
 * If there's only one argument, expands to nothing. If there is more
 * than one argument, expands to a '<<' followed by everything but
 * the first argument. Only supports up to 2 arguments but can be
 * trivially expanded.
 */
#define OPENAGE_ENS_REST(...) OPENAGE_ENS_REST_HELPER(OPENAGE_ENS_NUM(__VA_ARGS__), __VA_ARGS__)
#define OPENAGE_ENS_REST_HELPER(qty, ...) OPENAGE_ENS_REST_HELPER2(qty, __VA_ARGS__)
#define OPENAGE_ENS_REST_HELPER2(qty, ...) OPENAGE_ENS_REST_HELPER_##qty(__VA_ARGS__)
#define OPENAGE_ENS_REST_HELPER_ONE(first) << ::openage::error::no_ensuring_message()
#define OPENAGE_ENS_REST_HELPER_TWOORMORE(first, ...) << __VA_ARGS__
#define OPENAGE_ENS_NUM(...) \
	OPENAGE_ENS_SELECT_2ND(__VA_ARGS__, TWOORMORE, ONE, throwaway)
#define OPENAGE_ENS_SELECT_2ND(a1, a2, a3, ...) a3

} // error

using error::Error;

} // openage::error
