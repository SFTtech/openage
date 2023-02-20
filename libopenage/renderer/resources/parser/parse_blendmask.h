// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "renderer/resources/terrain/blendpattern_info.h"
#include "util/path.h"

namespace openage::renderer::resources::parser {

/**
 * Parse an blending table definition from a .blmask format file.
 *
 * @param file Path to the blendmask file.
 *
 * @return The corresponding blendmask definition.
 */
BlendPatternInfo parse_blendmask_file(const util::Path &file);

} // namespace openage::renderer::resources::parser
