// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UTIL_DEMANGLE_H_
#define OPENAGE_UTIL_DEMANGLE_H_

#include <string>
#include <typeinfo>
#include <utility>

namespace openage {
namespace util {

/**
 * demangle the C-string type name in parameter and return a std::string
 */
std::string demangle(const char* name);

/**
 * demangle the type name of the given parameter and return a std::string
 */
template <typename T>
std::string demangle(const T& t) {

    return demangle(typeid(t).name());
}

template <typename T>
std::string demangle() {

    return demangle(typeid(T).name());
}

} // namespace openage
} // namespace util

#endif

