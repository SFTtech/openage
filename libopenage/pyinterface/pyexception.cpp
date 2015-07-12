// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "pyexception.h"

#include "../log/log.h"

namespace openage {
namespace pyinterface {


void PyExceptionBacktrace::get_symbols(std::function<void (const error::backtrace_symbol *)> cb, bool reversed) const {
	if (reversed) {
		pyexception_bt_get_symbols.call(this->ref, cb);
	} else {
		// pyexception_bt_get_symbols gives us the symbols in reverse order, we'll have to use a std::vector
		// to flip it.

		std::vector<error::backtrace_symbol> symbols;
		pyexception_bt_get_symbols.call(this->ref, [&symbols](const error::backtrace_symbol *symbol){
			symbols.emplace_back(*symbol);
		});

		for(size_t idx = symbols.size(); idx-- > 0;) {
			cb(&symbols[idx]);
		}
	}
}


void PyException::init_backtrace() {
	this->backtrace = std::make_shared<PyExceptionBacktrace>(this->py_obj.get_ref());
}


std::string PyException::type_name() const {
	std::string result;

	result.append(this->py_obj.modulename());
	result.push_back('.');
	result.append(this->py_obj.classname());

	return result;
}


PyIfFunc<void, void *, Func<void, const error::backtrace_symbol *>> pyexception_bt_get_symbols;


}} // openage::pyinterface
