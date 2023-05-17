// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "util/path.h"

namespace openage::renderer::tests {

/**
 * Loads .sprite and .texture meta information files and displays
 * subtextures from the attached texture images.
 *
 * @param path Path to the openage asset directory.
 */
void renderer_demo_1(const util::Path &path);

} // namespace openage::renderer::tests
