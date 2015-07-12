// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UTIL_LANGUAGE_H_
#define OPENAGE_UTIL_LANGUAGE_H_


/*
 * Some general-purpose utilities related to the C++ language.
 */


namespace openage {
namespace util {


/**
 * Simple wrapper type that contains a function pointer.
 * Needed as workaround for http://stackoverflow.com/questions/31040075
 */
template<typename ReturnType, typename ... ArgTypes>
class FunctionPtr {
public:
	/**
	 * Implicit conversion from raw type.
	 */
	FunctionPtr(ReturnType (*ptr)(ArgTypes ...)) :
		ptr{ptr} {}

	ReturnType (*ptr)(ArgTypes ...);
};


/**
 * Wrapping a function call with ignore_result makes it more clear to
 * readers, compilers and linters that you are, in fact, ignoring the
 * function's return value on purpose.
 */
template<typename T>
inline void ignore_result(T /* unused result */) {}


}} // openage::util

#endif
