// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

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

} // namespace openage::renderer
