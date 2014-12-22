// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_TERRAIN_TERRAIN_OUTLINE_H_
#define OPENAGE_TERRAIN_TERRAIN_OUTLINE_H_

#include <memory>

#include "../coord/tile.h"

namespace openage {

class Texture;

/**
 * Generate a isometric square outline texture
 */
std::unique_ptr<Texture> square_outline(coord::tile_delta foundation_size);

/**
 * Generate a isometric circle outline texture
 */
std::unique_ptr<Texture> radial_outline(float radius);

} // namespace openage

#endif
