// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "util/path.h"

namespace openage::renderer::tests {

/**
 * Show the usage of uniform buffers in the renderer.
 *     - Window creation
 *     - Loading shaders
 *     - Vertex data and mesh creation
 *     - Uniform buffer assignment
 *     - Uniform buffer update
 *
 * @param path Path to the openage asset directory.
 */
void renderer_demo_5(const util::Path &path);

} // namespace openage::renderer::tests
