// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "renderer/resources/terrain/blendpattern_info.h"


namespace openage {
namespace util {
class Path;
}

namespace renderer::resources {
class AssetCache;

namespace parser {

/**
 * Parse an blending table definition from a .blmask format file.
 *
 * @param file Path to the blendmask file.
 * @param cache Cache of already loaded assets (optional).
 *
 * @return The corresponding blendmask definition.
 */
BlendPatternInfo parse_blendmask_file(const util::Path &file,
                                      const std::shared_ptr<AssetCache> &cache = nullptr);

} // namespace parser
} // namespace renderer::resources
} // namespace openage
