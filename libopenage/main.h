// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

// pxd: from libcpp.string cimport string
#include <string>
// pxd: from libc.stdint cimport int32_t
#include <cstdint>

namespace openage {


/**
 * Used for passing arguments to run_game.
 *
 * TODO remove lots of these.
 *
 * pxd:
 *
 * cppclass main_arguments:
 *     string version
 *     string data_directory
 *     int32_t fps_limit
 */
struct main_arguments {
	std::string version;
	std::string data_directory;
	std::int32_t fps_limit;
};


/**
 * runs the game.
 *
 * pxd: int run_game(main_arguments args) except +
 */
int run_game(const main_arguments &args);

} // openage
