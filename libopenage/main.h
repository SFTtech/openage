// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#pragma once

// pxd: from libcpp cimport bool
// pxd: from libcpp.string cimport string
// pxd: from libcpp.vector cimport vector
#include <string>

// pxd: from libopenage.util.path cimport Path
#include "util/path.h"

#include <vector>

#include "util/compiler.h"

namespace openage {

/**
 * Used for passing arguments to run_game.
 *
 * pxd:
 *
 * cppclass main_arguments:
 *     Path root_path
 *     bool gl_debug
 *     bool headless
 *     vector[string] mods
 *     int width
 *     int height
 */
struct main_arguments {
	util::Path root_path;
	bool gl_debug;
	bool headless;
	std::vector<std::string> mods;
	int width;
	int height;
};


/**
 * runs the game.
 *
 * pxd: int run_game(const main_arguments &args) except +
 */
OAAPI int run_game(const main_arguments &args);

} // namespace openage
