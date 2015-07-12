// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_CROSSPLATFORM_OS_H_
#define OPENAGE_CROSSPLATFORM_OS_H_

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

#endif
