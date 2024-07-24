// Copyright 2021-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <memory>

#include "renderer/resources/animation/animation_info.h"
#include "renderer/resources/animation/layer_info.h"


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
 * Definition according to doc/media/openage/sprite_format_spec.md.
 */
struct LayerData {
	size_t layer_id;
	display_mode mode{display_mode::OFF};
	size_t position{0};
	float time_per_frame{0.0};
	float replay_delay{0.0};
};

struct AngleData {
	size_t degree;
	int mirror_from{-1};
};

struct FrameData {
	size_t index;
	size_t angle;
	size_t layer_id;
	size_t texture_id;
	size_t subtex_id;
};

/**
 * Parse an Animation2d definition from a .sprite format file.
 *
 * @param file Path to the sprite file.
 * @param cache Cache of already loaded assets (optional).
 *
 * @return The corresponding animation definition.
 */
Animation2dInfo parse_sprite_file(const util::Path &file,
                                  const std::shared_ptr<AssetCache> &cache = nullptr);

} // namespace parser
} // namespace renderer::resources
} // namespace openage
