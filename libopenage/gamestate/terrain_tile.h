// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <optional>

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
	 * Path to the terrain asset used by this tile.
	 *
	 * TODO: Remove this and fetch the asset path from the terrain definition.
	 */
	std::string terrain_asset_path;

	/**
	 * Height of this tile on the terrain.
	 */
	terrain_elevation_t elevation;
};

} // namespace openage::gamestate
