// Copyright 2020-2020 the openage authors. See copying.md for legal info.

#pragma once

// pxd: from libcpp.map cimport map
#include <map>
// pxd: from libcpp.string cimport string
#include <string>

#include "../util/compiler.h"


namespace openage::versions {

/**
 * return a mapping of tool name to tool version,
 * for various components of the engine.
 *
 * pxd:
 * map[string,string] get_version_numbers() except +
 */
OAAPI std::map<std::string,std::string> get_version_numbers();

} // namespace openage::versions
