// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <limits>

#include "coord/scene.h"


/**
 * Hardcoded definitions for parameters used in the renderer.
 *
 * May be moved to configuration files in the future.
 */
namespace openage::renderer {

/**
 * Origin point of the 3D scene.
 */
constexpr coord::scene3 SCENE_ORIGIN = coord::scene3{0, 0, 0};

/**
 * Maximum priority value for a render pass layer.
 */
static constexpr int64_t LAYER_PRIORITY_MAX = std::numeric_limits<int64_t>::max();

} // namespace openage::renderer
