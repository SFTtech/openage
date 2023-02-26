// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <optional>

#include "renderer/resources/terrain/terrain_info.h"
#include "util/path.h"

namespace openage::renderer {
class Texture2d;

namespace resources::parser {

using texture_cache_t = std::unordered_map<std::string, std::shared_ptr<Texture2d>>;
using bltable_cache_t = std::unordered_map<std::string, std::shared_ptr<BlendTableInfo>>;

/**
 * Containers for the raw data.
 *
 * Definition according to doc/media/openage/terrain_format_spec.md.
 */
struct BlendtableData {
	size_t table_id;
	std::string path;
};

struct TerrainLayerData {
	size_t layer_id;
	terrain_display_mode mode{terrain_display_mode::OFF};
	size_t position{0};
	float time_per_frame{0.0};
	float replay_delay{0.0};
};

struct TerrainFrameData {
	size_t index;
	size_t layer_id;
	size_t texture_id;
	size_t subtex_id;
	size_t priority;
	std::optional<size_t> blend_mode;
};

/**
 * Parse an Terrain definition from a .terrain format file.
 *
 * @param file Path to the terrain file.
 * @param texture_cache Cache of already loaded textures (optional).
 * @param table_cache Cache of already loaded blending tables (optional).
 *
 * @return The corresponding terrain definition.
 */
TerrainInfo parse_terrain_file(const util::Path &file,
                               const texture_cache_t &texture_cache = {},
                               const bltable_cache_t &table_cache = {});

} // namespace resources::parser
} // namespace openage::renderer
