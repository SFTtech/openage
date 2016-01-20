// Copyright 2014-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <string>
#include <vector>

namespace openage {

/**
 * similar in its goals to Python's subprocess module
 */
namespace subprocess {

/**
 * returns true IFF the file is executable
 */
bool is_executable(const char *filename);

/**
 * finds the path to an executable by evaluating env[PATH]
 */
std::string which(const char *name);

/**
 * starts the process at argv[0]
 * passes argv as argv
 *
 * on failure, -1 is returned.
 *
 * @param argv               must consist of at least one non-empty string,
 *                           and end with a nullptr. this is not verified.
 *
 * @param wait               decides whether we wait for the process to finish.
 *                           if true, returns the process's return value
 *                           if false, returns 0
 *
 * @param redirect_stdout_to if not nullptr, the given file is opened and
 *                           substituted for the process's stdout.
 *                           the existing file is overwritten.
 */
int call(const std::vector<const char *> &argv,
         bool wait=false,
         const char *redirect_stdout_to = nullptr);

} // namespace subprocess
} // namespace openage
