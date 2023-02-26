// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "renderer/resources/terrain/blendpattern_info.h"
#include "util/path.h"

namespace openage::renderer {
class Texture2d;

namespace resources::parser {

using texture_cache_t = std::unordered_map<std::string, std::shared_ptr<Texture2d>>;

/**
 * Parse an blending table definition from a .blmask format file.
 *
 * @param file Path to the blendmask file.
 * @param texture_cache Cache of already loaded textures (optional).
 *
 * @return The corresponding blendmask definition.
 */
BlendPatternInfo parse_blendmask_file(const util::Path &file,
                                      const texture_cache_t &texture_cache = {});

} // namespace resources::parser
} // namespace openage::renderer
