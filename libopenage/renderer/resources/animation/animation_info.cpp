// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#include "animation_info.h"

namespace openage::renderer::resources {

Animation2dInfo::Animation2dInfo(const float scalefactor,
                                 std::vector<Texture2dInfo> &textures,
                                 std::vector<LayerInfo> &layers) :
	scalefactor{scalefactor},
	textures{textures},
	layers{layers} {}

float Animation2dInfo::get_scalefactor() const {
	return this->scalefactor;
}

size_t Animation2dInfo::get_texture_count() const {
	return this->textures.size();
}

const Texture2dInfo &Animation2dInfo::get_texture(size_t idx) const {
	return this->textures[idx];
}

size_t Animation2dInfo::get_layer_count() const {
	return this->layers.size();
}

const LayerInfo &Animation2dInfo::get_layer(size_t idx) const {
	return this->layers[idx];
}

} // namespace openage::renderer::resources
