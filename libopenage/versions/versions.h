// Copyright 2020-2020 the openage authors. See copying.md for legal info.

#pragma once

// pxd: from libcpp.string cimport string
// pxd: from libcpp.map cimport map
#include <map>

#include "../util/compiler.h"


namespace openage {

/**
 * gets some int
 *
 * pxd:
 *
 * map[string,string] get_version_numbers() except +
 */
OAAPI std::map<std::string,std::string> get_version_numbers();
}
