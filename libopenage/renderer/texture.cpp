// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include <cstring>

#include "texture.h"

#include "../error/error.h"


namespace openage {
namespace renderer {

Texture::Texture(resources::TextureInfo info)
	: info(info) {}

Texture::~Texture() {}

const resources::TextureInfo& Texture::get_info() const {
	return this->info;
}

}} // namespace openage::renderer
