// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "../../util/compiler.h"
// pxd: from libopenage.util.path cimport Path


namespace openage {
namespace util {
class Path;
} // namespace util

namespace gamestate::tests {

// pxd: void simulation_demo(int demo_id, Path path) except +
OAAPI void simulation_demo(int demo_id, const util::Path &path);

} // namespace gamestate::tests
} // namespace openage
