// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include "util/path.h"

namespace openage::renderer::tests {

/**
 * Show the window creation functionality of the level 1 renderer:
 *     - Window creation
 *     - Loading shaders
 *     - Creating a render pass
 *     - Creating a renderable from a mesh
 *
 * @param path Path to the project rootdir.
 */
void renderer_demo_0(const util::Path &path);

} // namespace openage::renderer::tests
