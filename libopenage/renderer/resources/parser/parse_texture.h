// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>

#include "renderer/resources/texture_info.h"


namespace openage {
namespace util {
class Path;
}

namespace renderer::resources::parser {

/**
 * Containers for the raw data.
 *
 * Definition according to doc/media/openage/texture_format_spec.md.
 */
struct SizeData {
	size_t width;
	size_t height;
};

struct PixelFormatData {
	pixel_format format;
	bool cbits;
};

struct SubtextureData {
	size_t xpos;
	size_t ypos;
	size_t xsize;
	size_t ysize;
	int xanchor;
	int yanchor;
};

/**
 * Parse a texture definition from a .texture format file.
 *
 * @param file Path to the texture file.
 *
 * @return The corresponding texture definition.
 */
Texture2dInfo parse_texture_file(const util::Path &file);

} // namespace renderer::resources::parser
} // namespace openage
