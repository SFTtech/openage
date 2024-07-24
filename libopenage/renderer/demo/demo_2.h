// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include "util/path.h"

namespace openage::renderer::tests {

/**
 * Show loading of .sprite and .texture meta information files and display
 * of subtextures from a texture atlas:
 *     - Window creation
 *     - Loading shaders
 *     - Loading resource files (.sprite and .texture)
 *     - Rendering of subtextures from a texture atlas (analogue to demo 1)
 *
 * @param path Path to the openage asset directory.
 */
void renderer_demo_2(const util::Path &path);

} // namespace openage::renderer::tests
