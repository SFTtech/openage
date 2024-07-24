// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <memory>
#include <optional>
#include <string>

#include "renderer/resources/terrain/layer_info.h"
#include "renderer/resources/terrain/terrain_info.h"


namespace openage {
namespace util {
class Path;
}

namespace renderer::resources {
class AssetCache;

namespace parser {

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
 * @param cache Cache of already loaded assets (optional).
 *
 * @return The corresponding terrain definition.
 */
TerrainInfo parse_terrain_file(const util::Path &file,
                               const std::shared_ptr<AssetCache> &cache = nullptr);

} // namespace parser
} // namespace renderer::resources
} // namespace openage
