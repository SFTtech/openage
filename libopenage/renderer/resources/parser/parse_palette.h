// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "renderer/resources/palette_info.h"
#include "util/path.h"

namespace openage::renderer::resources::parser {

/**
 * Parse an palette definition from a .opal format file.
 *
 * @param file Path to the palette file.
 *
 * @return The corresponding palette definition.
 */
PaletteInfo parse_palette_file(const util::Path &file);

} // namespace openage::renderer::resources::parser
