// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "renderer/resources/terrain/blendtable_info.h"
#include "util/path.h"

namespace openage::renderer::resources {
class AssetCache;

namespace parser {

/**
 * Containers for the raw data.
 *
 * Definition according to doc/media/openage/blendtable_format_spec.md.
 */
struct PatternData {
	size_t pattern_id;
	std::string path;
};

/**
 * Parse an blending table definition from a .bltable format file.
 *
 * @param file Path to the blendtable file.
 * @param cache Cache of already loaded assets (optional).
 *
 * @return The corresponding blendtable definition.
 */
BlendTableInfo parse_blendtable_file(const util::Path &file,
                                     const std::shared_ptr<AssetCache> &cache = nullptr);

} // namespace parser
} // namespace openage::renderer::resources
