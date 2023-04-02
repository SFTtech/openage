// Copyright 2021-2023 the openage authors. See copying.md for legal info.

#include "animation_info.h"

namespace openage::renderer::resources {

Animation2dInfo::Animation2dInfo(const float scalefactor,
                                 std::vector<std::shared_ptr<Texture2dInfo>> &textures,
                                 std::vector<LayerInfo> &layers) :
	scalefactor{scalefactor},
	texture_infos{textures},
	layers{layers} {}

float Animation2dInfo::get_scalefactor() const {
	return this->scalefactor;
}

size_t Animation2dInfo::get_texture_count() const {
	return this->texture_infos.size();
}

const std::shared_ptr<Texture2dInfo> &Animation2dInfo::get_texture(size_t idx) const {
	return this->texture_infos[idx];
}

size_t Animation2dInfo::get_layer_count() const {
	return this->layers.size();
}

const LayerInfo &Animation2dInfo::get_layer(size_t idx) const {
	return this->layers[idx];
}

} // namespace openage::renderer::resources
