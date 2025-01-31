// Copyright 2025-2025 the openage authors. See copying.md for legal info.

#pragma once

#include "util/path.h"

namespace openage::renderer::tests {

/**
 * Demonstrate the shader template system for shader generation.
 *     - Window creation
 *     - Create a shader template
 *     - Load shader snippets (command) from files
 *     - Generate shader sources from the template
 *     - Creating a render pass
 *     - Creating a renderable from a mesh
 *
 * @param path Path to the project rootdir.
 */
void renderer_demo_7(const util::Path &path);

} // namespace openage::renderer::tests
