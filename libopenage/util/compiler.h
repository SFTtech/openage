// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UTIL_COMPILER_H_
#define OPENAGE_UTIL_COMPILER_H_

/*
 * Some general-purpose utilities related to the C++ compiler and standard
 * library implementations.
 *
 * May contain some platform-dependent code.
 */

// pxd: from libcpp.string cimport string
#include <string>

/*
 * Branch prediction tuning.
 * the expression is expected to be true (=likely) or false (=unlikely).
 *
 * btw, this implementation was taken from the Linux kernel.
 */
#define likely(x)    __builtin_expect(!!(x), 1)
#define unlikely(x)  __builtin_expect(!!(x), 0)

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
std::string symbol_name(const void *addr, bool require_exact_addr=true, bool no_pure_addrs=false);


/**
 * Returns true if the address is the (exact) address of a symbol.
 */
bool is_symbol(const void *addr);


}} // openage::util


#endif
