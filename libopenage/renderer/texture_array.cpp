// Copyright 2018 the openage authors. See copying.md for legal info.

#include "texture_array.h"


namespace openage {
namespace renderer {

Texture2dArray::Texture2dArray(resources::Texture2dInfo info)
	: layer_info(info) {}

Texture2dArray::~Texture2dArray() {}

resources::Texture2dInfo const& Texture2dArray::get_info() const {
	return this->layer_info;
}

}} // namespace openage::renderer
