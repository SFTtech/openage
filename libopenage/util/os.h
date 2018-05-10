// Copyright 2014-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <string>

namespace openage {

/**
 * similar in its goals to Python's os module
 */
namespace os {

/**
 * reads a symlink
 */
std::string read_symlink(const char *path);

/**
 * returns openage's executable name
 */
std::string self_exec_filename();

/**
 * tries to xdg-open the file
 */
int execute_file(const char *path, bool background=true);

} // namespace os
} // namespace openage
