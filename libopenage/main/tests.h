// Copyright 2018-2018 the openage authors. See copying.md for legal info.

#pragma once

// pxd: from libopenage.util.path cimport Path
#include "../util/path.h"


namespace openage::main::tests {

// pxd: void engine_demo(int demo_id, Path path) except +
void engine_demo(int demo_id, const util::Path &path);

} // openage::main::tests
