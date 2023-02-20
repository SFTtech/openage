// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "common.h"

namespace openage::renderer::resources::parser {

/**
 * Parse the texture attribute.
 *
 * @param args Arguments from the line with a \p texture attribute.
 *             The first argument is expected to be the attribute keyword.
 *
 * @return Struct containing the attribute data.
 */
TextureData parse_texture(const std::vector<std::string> &args) {
	// TODO: Splitting at the space char assumes that the path string contains no
	// space. While the space char is not allowed because of nyan naming requirements,
	// it should result in an error if wrongly used here.
	TextureData texture;

	texture.texture_id = std::stoul(args[1]);

	// Call substr() to get rid of the quotes
	texture.path = args[2].substr(1, args[2].size() - 2);

	return texture;
}

/**
 * Parse the scalefactor attribute.
 *
 * @param args Arguments from the line with a \p scalefactor attribute.
 *             The first argument is expected to be the attribute keyword.
 *
 * @return Scalefactor value.
 */
float parse_scalefactor(const std::vector<std::string> &args) {
	return std::stof(args[1]);
}

} // namespace openage::renderer::resources::parser