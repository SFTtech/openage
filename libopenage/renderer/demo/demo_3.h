// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include "util/path.h"

namespace openage::renderer::tests {

/**
 * Show off the render stages in the level 2 renderer and the camera
 * system.
 *     - Window creation
 *     - Creating a camera
 *     - Initializing the level 2 render stages: skybox, terrain, world, screen
 *     - Adding renderables to the render stages via the render factory
 *     - Moving camera with mouse/keyboard callbacks
 *
 * @param path Path to the project rootdir.
 */
void renderer_demo_3(const util::Path &path);

} // namespace openage::renderer::tests
