// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_ERROR_BACKTRACE_H_
#define OPENAGE_ERROR_BACKTRACE_H_

// pxd: from libcpp cimport bool as cppbool

#include <vector>
// pxd: from libcpp.string cimport string
#include <string>
#include <iostream>
// pxd: from cpp.pyinterface.functional cimport Func1
#include <functional>

namespace openage {
namespace error {


// Note: This header file describes a fixed frontend for all sorts of backend backtrace-getters.


/**
 * A single symbol, as determined from a program counter, and returned by Backtrace::get_symbols().
 *
 * pxd:
 *
 * cppclass backtrace_symbol:
 *     string filename
 *     int lineno
 *     string functionname
 *     void *pc
 *
 * ctypedef const backtrace_symbol *backtrace_symbol_constptr
 */
struct backtrace_symbol {
	std::string filename;      // empty if unknown
	unsigned int lineno;       // 0 if unknown
	std::string functionname;  // empty if unknown
	void *pc;                  // nullptr if unknown
};


std::ostream &operator <<(std::ostream &os, const backtrace_symbol &bt_sym);


/**
 * Abstract class for objects that provide backtrave information through
 * get_symbols().
 * For a non-abstract implementation, see stackanalyzer.h.
 *
 * pxd:
 *
 * cppclass Backtrace:
 *    void get_symbols(Func1[void, backtrace_symbol *] callback, cppbool reversed) except +
 */
class Backtrace {
public:
	/**
	 * Returns (via the callback) symbolic names for all stack frames,
	 * to its best knowledge.
	 *
	 * The most recent call is returned last (alike Python).
	 *
	 * @param cb
	 *    is called for every symbol in the backtrace, starting with the top-most
	 *    frame.
	 * @param reversed
	 *    if true, the most recent call is given last.
	 */
	virtual void get_symbols(std::function<void (const backtrace_symbol *)> cb,
	                         bool reversed=true) const = 0;

	/**
	 * Removes all the lower frames that are also present in the current stack.
	 *
	 * Designed to be used in catch clauses, to simulate stack trace collection
	 * from throw to catch, instead of from throw to the process entry point.
	 *
	 * Defaults to no-op.
	 */
	virtual void trim_to_current_stack_frame() {};

	virtual ~Backtrace() = default;
};


std::ostream &operator <<(std::ostream &os, const Backtrace &bt);


}} // openage::error

#endif
