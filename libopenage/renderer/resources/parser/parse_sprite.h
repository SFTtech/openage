// Copyright 2021-2021 the openage authors. See copying.md for legal info.

#pragma once

#include "../../../util/path.h"
#include "../../animation.h"

namespace openage::renderer::resources::parser {

/**
 * Containers for the raw data.
 *
 * Definition according to doc/media/openage/sprite_format_spec.md.
 */
struct ImageData {
	size_t id;
	std::string path;
};

struct LayerData {
	size_t id;
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
	size_t image_id;
	size_t xpos;
	size_t ypos;
	size_t xsize;
	size_t ysize;
	size_t xhotspot;
	size_t yhotspot;
};

/**
 * Parse an Animation2d definition from a .sprite format file.
 *
 * @param file Path to the sprite file.
 *
 * @return The corresponding animation definition.
 */
Animation2d parse_sprite(util::Path &file);

} // namespace openage::renderer::resources::parser