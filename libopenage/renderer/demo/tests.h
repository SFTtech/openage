// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "../../util/compiler.h"
// pxd: from libopenage.util.path cimport Path
#include "../../util/path.h"


namespace openage::renderer::tests {

// pxd: void renderer_demo(int demo_id, Path path) except +
OAAPI void renderer_demo(int demo_id, const util::Path &path);

} // namespace openage::renderer::tests
