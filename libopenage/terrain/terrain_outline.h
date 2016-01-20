// Copyright 2014-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "../coord/tile.h"

namespace openage {

class Texture;

/**
 * Generate a isometric square outline texture
 */
std::shared_ptr<Texture> square_outline(coord::tile_delta foundation_size);

/**
 * Generate a isometric circle outline texture
 */
std::shared_ptr<Texture> radial_outline(float radius);

} // namespace openage
