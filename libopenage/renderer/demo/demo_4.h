// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include "util/path.h"

namespace openage::renderer::tests {

/**
 * Show the timing of animation keyframes with a simulation clock.
 *     - Window creation
 *     - Loading shaders
 *     - Creating the simulation clock
 *     - Loading .sprite and .texture files and accessing subtextures
 *     - Update of animation keyframes via shader uniforms
 *     - Switching to correct animation keyframe by using the clock.
 *
 * @param path Path to the openage asset directory.
 */
void renderer_demo_4(const util::Path &path);

} // namespace openage::renderer::tests
