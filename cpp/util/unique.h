// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UTIL_UNIQUE_H_
#define OPENAGE_UTIL_UNIQUE_H_

#include <type_traits>
#include <utility>
#include <memory>

namespace openage {
namespace util {

//essentially the implementation from libc++, clang

namespace _unique_help {
//make_unique is different for T, T[], and T[N] (ill formed for T[N])
//These helper structs allow proper specializations

template<class T>
struct unique_rval {
	using single_return = std::unique_ptr<T>;
};

template<class T>
struct unique_rval<T[]> {
	using multi_return = std::unique_ptr<T[]>;
};

//causes compile fail for fixed_type arrays
//since result is unspecified by standard.
template<class T, size_t N>
struct unique_rval<T[N]> {
	using known_bound = std::unique_ptr<T[N]>;
	static_assert(N < 0, "make_unique does not work for fixed_size arrays");
};

} //namespace _unique_help

/**
 * Creates a unique_ptr pointing towards new T(Args...)
 * Implementation of http://en.cppreference.com/w/cpp/memory/unique_ptr/make_unique
 *
 * @param Args The arguments forwarded to the constructor of T
 */
template<class T, class... Args>
inline typename _unique_help::unique_rval<T>::single_return
make_unique(Args&&... args) {
	return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

/**
 * Creates a unique_ptr pointing towards a new T[n]
 * Implementation of http://en.cppreference.com/w/cpp/memory/unique_ptr/make_unique
 *
 * @param n The size of the array being allocated
 */
template<class T>
inline typename _unique_help::unique_rval<T>::multi_return
make_unique(size_t n) {
	using create_type = typename std::remove_extent<T>::type;
	return std::unique_ptr<T>(new create_type[n]);
}

template<class T, class... Args>
typename _unique_help::unique_rval<T>::known_bound
make_unique(Args&&...) {
	return std::unique_ptr<T>(nullptr);
}

} //namespace util
} //namespace openage

#endif
