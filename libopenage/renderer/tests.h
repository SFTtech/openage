// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#pragma once

// pxd: from libopenage.util.path cimport Path
#include "../util/path.h"


namespace openage {
namespace renderer {
namespace tests {

// pxd: void renderer_demo(int demo_id, Path path) except +
void renderer_demo(int demo_id, util::Path path);

}}} // openage::renderer::tests
