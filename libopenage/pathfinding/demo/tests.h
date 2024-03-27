// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include "../../util/compiler.h"
// pxd: from libopenage.util.path cimport Path


namespace openage {
namespace util {
class Path;
} // namespace util

namespace path::tests {

// pxd: void path_demo(int demo_id, Path path) except +
OAAPI void path_demo(int demo_id, const util::Path &path);

} // namespace path::tests
} // namespace openage
