// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include "util/path.h"

namespace openage::renderer::tests {

/**
 * Shows the level 1 renderer's ability to create textured renderable objects and
 * allow basic interaction with them via mouse/key callbacks:
 *     - Window creation
 *     - Loading shaders
 *     - Creating multiple render passes
 *     - Creating multiple renderables for a render pass
 *     - Configuring and setting uniforms for renderables
 *     - Changing uniforms via mouse/key callbacks
 *
 * @param path Path to project rootdir.
 */
void renderer_demo_1(const util::Path &path);

} // namespace openage::renderer::tests
