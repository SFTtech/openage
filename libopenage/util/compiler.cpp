// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "compiler.h"

#ifndef _MSC_VER
#include <cxxabi.h>
#include <dlfcn.h>
#endif

#include "strings.h"

#include <iostream>

namespace openage {
namespace util {


std::string demangle(const char *symbol) {
#ifdef _MSC_VER
	// TODO: demangle names for MSVC; Possibly using UnDecorateSymbolName
	// https://msdn.microsoft.com/en-us/library/windows/desktop/ms681400(v=vs.85).aspx
	return symbol;
#else
	int status;
	char *buf = abi::__cxa_demangle(symbol, nullptr, nullptr, &status);

	if (status != 0) {
		return symbol;
	} else {
		std::string result{buf};
		free(buf);
		return result;
	}
#endif
}


std::string addr_to_string(const void *addr) {
	return sformat("[%p]", addr);
}


std::string symbol_name(const void *addr, bool require_exact_addr, bool no_pure_addrs) {
#ifdef _MSC_VER
	// TODO: implement symbol_name for MSVC; Possibly using SymFromAddr
	// https://msdn.microsoft.com/en-us/library/windows/desktop/ms681323(v=vs.85).aspx
	return no_pure_addrs ? "" : addr_to_string(addr);
#else
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
#endif
}


bool is_symbol(const void *addr) {
#ifdef _MSC_VER
	// TODO: Get dladdr equivalent for MSVC.
	return true;
#else
	Dl_info addr_info;

	if (dladdr(addr, &addr_info) == 0) {
		return false;
	}

	return (addr_info.dli_saddr == addr);
#endif
}


}} // openage::util
