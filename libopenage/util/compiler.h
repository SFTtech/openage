// Copyright 2014-2019 the openage authors. See copying.md for legal info.

#pragma once

/*
 * Some general-purpose utilities related to the C++ compiler and standard
 * library implementations.
 *
 * May contain some platform-dependent code.
 */
#include <ciso646>

// pxd: from libcpp.string cimport string
#include <string>

/**
 * DLL entry-point decorations.
 */
#if defined(_WIN32)
#if defined(libopenage_EXPORTS)
#define OAAPI __declspec(dllexport)
#else
#define OAAPI __declspec(dllimport)
#endif /* libopenage_EXPORTS */
#else
#define OAAPI
#endif

#if defined(_MSC_VER)
#ifndef HAVE_SSIZE_T
// ssize_t is defined the same as Python's definition it in pyconfig.h.
// This is necessary to facilitate the build and link procedure using MSVC.
#ifdef _WIN64
typedef __int64 ssize_t;
#else
typedef int ssize_t;
#endif
#define HAVE_SSIZE_T 1
#endif // HAVE_SSIZE_T
#endif // _MSC_VER

/*
 * Branch prediction tuning.
 * the expression is expected to be true (=likely) or false (=unlikely).
 *
 * btw, this implementation was taken from the Linux kernel.
 */
#if defined(__GNUC__)
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif


/**
 * Software breakpoint if you're too lazy
 * to add it in gdb but instead wanna add it into the code directly.
 */
#ifdef _WIN32
#define BREAKPOINT __debugbreak()
#else
#include <signal.h>
#define BREAKPOINT raise(SIGTRAP)
#endif


/**
 * As C++ is such a shit language, we found this awesome
 * way to display a type of some variable as a warning.
 *
 * Other hacks like wrong assignments work as well, but crash
 * the compilation by error.
 */
#define TYPEINFO(var) printf("%d", var)


namespace openage {
namespace util {

/**
 * Demangles a symbol name.
 *
 * On failure, the mangled symbol name is returned.
 */
std::string demangle(const char *symbol);


/**
 * Returns the (demangled) symbol name for a given address.
 *
 * If no exact match can be found,
 * if require_exact_addr == true (default):
 *   if no_pure_addrs == false (default):
 *     a pure hex address string is returned
 *   else:
 *     an empty string is returned
 * if require_exact_addr == false:
 *   name of closest symbol + offset hex string is returned
 *
 * pxd: string symbol_name(const void *addr) except +
 */
OAAPI std::string
symbol_name(const void *addr, bool require_exact_addr = true, bool no_pure_addrs = false);


/**
 * Returns true if the address is the (exact) address of a symbol.
 */
bool is_symbol(const void *addr);


/**
 * Returns the string representation of the given type.
 */
template<typename T>
std::string typestring() {
	return demangle(typeid(T).name());
}


} // namespace util
} // namespace openage
