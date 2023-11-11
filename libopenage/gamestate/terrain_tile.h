// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>

#include <nyan/nyan.h>

#include "util/fixed_point.h"


namespace openage::gamestate {

using terrain_elevation_t = util::FixedPoint<uint64_t, 16>;

/**
 * A single terrain tile.
 */
struct TerrainTile {
	/**
     * Terrain definition used by this tile.
     */
	nyan::Object terrain;

	/**
     * Height of this tile on the terrain.
     */
	terrain_elevation_t elevation;
};

} // namespace openage::gamestate
