// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

// pxd: from libcpp cimport bool
// pxd: from libcpp.string cimport string
#include <string>
// pxd: from libc.stdint cimport int32_t
#include <cstdint>
// pxd: from libcpp.memory cimport unique_ptr
#include <memory>

// pxd: from libopenage.util.path cimport Path
#include "util/path.h"


namespace openage {

/**
 * Used for passing arguments to run_game.
 *
 * pxd:
 *
 * cppclass main_arguments:
 *     unique_ptr[Path] asset_path
 *     int32_t fps_limit
 *     bool gl_debug
 */
struct main_arguments {
	std::unique_ptr<util::Path> asset_path;
	int32_t fps_limit;
	bool gl_debug;
};


/**
 * runs the game.
 *
 * pxd: int run_game(main_arguments args) except +
 */
int run_game(const main_arguments &args);

} // openage
