// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <string>

namespace openage {
namespace util {


/**
 * method that behaves similar to python's "repr".
 * puts the string in quotes and escapes all sorts of stuff inside.
 */
std::string repr(const std::string &arg);


} // namespace util
} // namespace openage
