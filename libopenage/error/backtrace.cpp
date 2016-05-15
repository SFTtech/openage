// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "backtrace.h"

#include <iostream>

#include <libopenage/config.h>

namespace openage {
namespace error {


/**
 * Prints a backtrace_symbol object.
 */
std::ostream &operator <<(std::ostream &os, const backtrace_symbol &bt_sym) {
	// imitate the looks of a Python traceback.
	os << "  File ";

	if (bt_sym.filename.empty()) {
		os << '?';
	} else {
		os << '"' << bt_sym.filename << '"';
	}

	if (bt_sym.lineno) {
		os << ", line " << bt_sym.lineno;
	}

	os << ", in ";

	if (bt_sym.functionname.empty()) {
		os << '?';
	} else {
		os << bt_sym.functionname;
	}

	if (bt_sym.pc != nullptr) {
		os << " [" << bt_sym.pc << "]";
	}

	return os;
}


/**
 * Prints an entire Backtrace object.
 */
std::ostream &operator <<(std::ostream &os, const Backtrace &bt) {
	// imitate the looks of a Python traceback.
	os << "Traceback (most recent call last):" << std::endl;

	bt.get_symbols([&os](const backtrace_symbol *symbol){
		os << *symbol << std::endl;
	}, true);

	return os;
}


}} // openage::error
