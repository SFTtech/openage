// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "compiler.h"

#include <cxxabi.h>
#include <dlfcn.h>

#include "strings.h"

#include <iostream>

namespace openage {
namespace util {


std::string demangle(const char *symbol) {
	int status;
	char *buf = abi::__cxa_demangle(symbol, nullptr, nullptr, &status);

	if (status != 0) {
		return symbol;
	} else {
		std::string result{buf};
		free(buf);
		return result;
	}
}


std::string addr_to_string(const void *addr) {
	return sformat("[%p]", addr);
}


std::string symbol_name(const void *addr, bool require_exact_addr, bool no_pure_addrs) {
	Dl_info addr_info;

	if (dladdr(addr, &addr_info) == 0) {
		// dladdr has... failed.
		return no_pure_addrs ? "" : addr_to_string(addr);
	} else {
		size_t symbol_offset = (size_t) addr - (size_t) addr_info.dli_saddr;

		if (addr_info.dli_sname == nullptr or (symbol_offset != 0 and require_exact_addr)) {
			return no_pure_addrs ? "" : addr_to_string(addr);
		}

		if (symbol_offset == 0) {
			// this is our symbol name.
			return demangle(addr_info.dli_sname);
		} else {
			return util::sformat("%s+0x%lx",
			                     demangle(addr_info.dli_sname).c_str(),
			                     symbol_offset);
		}
	}
}


bool is_symbol(const void *addr) {
	Dl_info addr_info;

	if (dladdr(addr, &addr_info) == 0) {
		return false;
	}

	return (addr_info.dli_saddr == addr);
}


}} // openage::util
