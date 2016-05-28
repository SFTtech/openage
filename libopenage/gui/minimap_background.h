// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <vector>
#include <tuple>
#include <memory>
#include <cstdint>

#include "../coord/chunk.h"
#include "../coord/tile.h"

namespace gamedata {
struct palette_color;
} // gamedata

namespace openage {

class Terrain;

namespace gui {

/**
 * @return terrain texture, size and bottom/right spacing in it in pixels
 */
std::tuple<std::unique_ptr<uint32_t[]>, int, int> generate_minimap_background(Terrain &terrain, const std::vector<gamedata::palette_color> &palette);

/**
 * @return hash for the state of what chunks are present
 */
size_t terrain_used_chunks_hash(const Terrain &terrain);

/**
 * @return sw corner and side length
 */
std::tuple<coord::chunk, coord::chunk_t> terrain_bounding_square(const Terrain &terrain);

/**
 * @return side length
 */
coord::chunk_t terrain_square_dimensions(const std::tuple<coord::chunk, coord::chunk_t> &square);

/**
 * @param square sw corner and side length
 * @return center
 */
coord::tile terrain_square_center(const std::tuple<coord::chunk, coord::chunk_t> &square);

}} // namespace openage::gui
