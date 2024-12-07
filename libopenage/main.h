// Copyright 2015-2024 the openage authors. See copying.md for legal info.

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
 * Window parameters struct.
 *
 * pxd:
 *
 * cppclass window_arguments:
 *     int width
 *     int height
 *     bool vsync
 *     string mode
 */
struct window_arguments {
	int width;
	int height;
	bool vsync;
	std::string mode;
};

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
 *     window_arguments window_args
 */
struct main_arguments {
	util::Path root_path;
	bool gl_debug;
	bool headless;
	std::vector<std::string> mods;
	window_arguments window_args;
};


/**
 * runs the game.
 *
 * pxd: int run_game(const main_arguments &args) except +
 */
OAAPI int run_game(const main_arguments &args);

} // namespace openage
