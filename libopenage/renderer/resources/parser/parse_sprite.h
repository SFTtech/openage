// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "renderer/animation.h"
#include "util/path.h"

namespace openage::renderer {
class Texture2d;

namespace resources::parser {

using texture_cache_t = std::unordered_map<std::string, std::shared_ptr<Texture2d>>;

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
 * @param texture_cache Cache of already loaded textures (optional).
 *
 * @return The corresponding animation definition.
 */
Animation2dInfo parse_sprite_file(const util::Path &file,
                                  const texture_cache_t &texture_cache = {});

} // namespace resources::parser
} // namespace openage::renderer
