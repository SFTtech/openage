// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_ERROR_ERROR_H_
#define OPENAGE_ERROR_ERROR_H_

#include <iostream>
#include <memory>
#include <stdexcept>
// pxd: from libcpp.string cimport string
#include <string>

// pxd: from cpp.log.message cimport message
#include "../log/log.h"
#include "../log/message.h"

// pxd: from cpp.error.backtrace cimport Backtrace
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
	 *     (default false).
	 *     The performance impacts should be not too bad, as only
	 *     program counter pointers are collected.
	 * @param ignore_cause
	 *     If true, a pointer to the causing exception is not
	 *     collected and stored (default false).
	 */
	Error(const log::message &msg, bool generate_backtrace=false, bool store_cause=true);


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


} // error

using error::Error;

} // openage::error

#endif
