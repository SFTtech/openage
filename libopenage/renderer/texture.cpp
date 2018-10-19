// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#include <cstring>

#include "texture.h"
#include "../error/error.h"


namespace openage {
namespace renderer {

Texture2d::Texture2d(const resources::Texture2dInfo& info)
	: info(info) {}

Texture2d::~Texture2d() = default;

const resources::Texture2dInfo& Texture2d::get_info() const {
	return this->info;
}

}} // namespace openage::renderer
