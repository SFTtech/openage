// Copyright 2018-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "../util/compiler.h"
// pxd: from libopenage.util.path cimport Path


namespace openage {
namespace util {
class Path;
} // namespace util

namespace main::tests {

// pxd: void engine_demo(int demo_id, Path path) except +
OAAPI void engine_demo(int demo_id, const util::Path &path);

} // namespace main::tests
} // namespace openage
