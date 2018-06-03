// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#include "compiler.h"

#ifndef _MSC_VER
#include <cxxabi.h>
#include <dlfcn.h>
#else
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <DbgHelp.h>
#endif

#include "strings.h"

#include <array>
#include <iostream>
#include <optional>
#include <mutex>

namespace openage {
namespace util {


std::string demangle(const char *symbol) {
#ifdef _MSC_VER
	// TODO: demangle names for MSVC; Possibly using UnDecorateSymbolName
	// https://msdn.microsoft.com/en-us/library/windows/desktop/ms681400(v=vs.85).aspx
	// Could it be that MSVC's typeid(T).name() already returns a demangled name? It seems that .raw_name() returns the mangled name
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

#ifdef _MSC_VER
namespace {


bool initialized_symbol_handler_successfully = false;


std::optional<std::string> symbol_name_win(const void *addr) {
	// Handle to the current process
	static HANDLE process_handle = INVALID_HANDLE_VALUE;
	static bool initialized_symbol_handler = false;

	// SymInitialize & SymFromAddr are, according to MSDN, not thread-safe.
	static std::mutex sym_mutex;
	std::lock_guard<std::mutex> sym_lock_guard{sym_mutex};

	// Initialize symbol handler for process, if it has not yet been initialized
	// If we are not succesful on the first try, leave it, since MSDN says that searching for symbol files is very time consuming
	if (!initialized_symbol_handler) {
		initialized_symbol_handler = true;

		process_handle = GetCurrentProcess();
		initialized_symbol_handler_successfully = SymInitialize(process_handle, nullptr, TRUE);
	}

	if (initialized_symbol_handler_successfully) {
		// The magic of winapi
		constexpr int name_buffer_size = 1024;
		constexpr int buffer_size = sizeof(SYMBOL_INFO) + name_buffer_size * sizeof(char);
		std::array<char, buffer_size> buffer;

		SYMBOL_INFO *symbol_info = reinterpret_cast<SYMBOL_INFO*>(buffer.data());

		symbol_info->SizeOfStruct = sizeof(SYMBOL_INFO);
		symbol_info->MaxNameLen = name_buffer_size;

		if (SymFromAddr(process_handle, reinterpret_cast<DWORD64>(addr), nullptr, symbol_info))	{
			return std::string(symbol_info->Name);
		}
	}

	return std::optional<std::string>();
}


}
#endif

std::string symbol_name(const void *addr, bool require_exact_addr, bool no_pure_addrs) {
#ifdef _MSC_VER

	auto symbol_name_result = symbol_name_win(addr);

	if (!initialized_symbol_handler_successfully ||
		!symbol_name_result.has_value()) {
		return no_pure_addrs ? "" : addr_to_string(addr);
	}

	return symbol_name_result.value();

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

	if (!initialized_symbol_handler_successfully) {
		return true;
	} else {
		return symbol_name_win(addr).has_value();
	}

#else
	Dl_info addr_info;

	if (dladdr(addr, &addr_info) == 0) {
		return false;
	}

	return (addr_info.dli_saddr == addr);
#endif
}


}} // openage::util
