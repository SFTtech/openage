// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#include "common.h"

#include <memory>


namespace openage::renderer::resources::parser {

size_t parse_version(const std::vector<std::string> &args) {
	return std::stoul(args[1]);
}

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

float parse_scalefactor(const std::vector<std::string> &args) {
	return std::stof(args[1]);
}

} // namespace openage::renderer::resources::parser
