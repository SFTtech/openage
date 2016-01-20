// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

// pxd: from libcpp.string cimport string
#include <string>

namespace openage {


/**
 * Used for passing arguments to run_game.
 *
 * TODO remove lots of these.
 *
 * pxd:
 *
 * cppclass main_arguments:
 *     string data_directory
 */
struct main_arguments {
	std::string data_directory;
};


/**
 * runs the game.
 *
 * pxd: int run_game(main_arguments args) except +
 */
int run_game(const main_arguments &args);

} // openage
