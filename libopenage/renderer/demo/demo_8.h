// Copyright 2025-2025 the openage authors. See copying.md for legal info.

#pragma once

#include "util/path.h"

namespace openage::renderer::tests {

/**
 * Show a demo of stencil testing.
 *     - GUI elements will create a stencil mask.
 *     - Background will only render in non-GUI areas.
 *
 * @param path Path to the project rootdir.
 */
void renderer_demo_8(const util::Path &path);

} // namespace openage::renderer::tests
