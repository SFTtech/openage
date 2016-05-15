// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

// pxd: from libcpp.string cimport string
#include <string>


namespace openage {


/**
 * A datastructure containing the build configuration information
 *
 * pxd:
 *
 * cppclass build_configuration:
 *     string global_asset_dir
 *     string config_options
 *     string build_src_dir
 *     string compiler
 *     string compiler_flags
 *     string cython_version
 *     build_configuration() except +
 */
struct build_configuration {
	std::string global_asset_dir;
	std::string config_options;
	std::string build_src_dir;
	std::string compiler;
	std::string compiler_flags;
	std::string cython_version;

	build_configuration();
};


} // namespace openage
