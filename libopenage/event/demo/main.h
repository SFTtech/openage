// Copyright 2018-2024 the openage authors. See copying.md for legal info.

#pragma once

#include "../../util/compiler.h"
// pxd: from libcpp cimport bool


namespace openage::event::demo {

// pxd: void curvepong(bool disable_gui, bool no_human) except +
OAAPI void curvepong(bool disable_gui, bool no_human);


} // namespace openage::event::demo
