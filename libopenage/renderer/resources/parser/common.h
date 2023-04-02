// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <string>
#include <vector>

/**
 * This file contains subparsers for arguments that work the
 * same across all formats.
 */
namespace openage::renderer::resources::parser {

/**
 * Containers for raw data.
 */

/**
 * Texture references.
 *
 * Used by:
 * .blmask: doc/media/openage/blendmask_format_spec.md.
 * .sprite: doc/media/openage/sprite_format_spec.md.
 * .terrain: doc/media/openage/terrain_format_spec.md.
 */
struct TextureData {
	size_t texture_id;
	std::string path;
};

/**
 * Subparsers for arguments.
 */

/**
 * Parse the file version attribute.
 *
 * @param args Arguments from the line with a \p version attribute.
 *             The first argument is expected to be the attribute keyword.
 *
 * @return Version number.
 */
size_t parse_version(const std::vector<std::string> &args);

/**
 * Parse the texture attribute.
 *
 * @param args Arguments from the line with a \p texture attribute.
 *             The first argument is expected to be the attribute keyword.
 *
 * @return Struct containing the attribute data.
 */
TextureData parse_texture(const std::vector<std::string> &args);

/**
 * Parse the scalefactor attribute.
 *
 * @param args Arguments from the line with a \p scalefactor attribute.
 *             The first argument is expected to be the attribute keyword.
 *
 * @return Scalefactor value.
 */
float parse_scalefactor(const std::vector<std::string> &args);

} // namespace openage::renderer::resources::parser
