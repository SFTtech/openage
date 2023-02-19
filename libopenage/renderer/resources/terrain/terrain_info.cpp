// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "terrain_info.h"

namespace openage::renderer::resources {

TerrainInfo::TerrainInfo(const std::shared_ptr<BlendTableInfo> &blendtable,
                         const float scalefactor,
                         std::vector<Texture2dInfo> &textures,
                         std::vector<TerrainLayerInfo> &layers) :
	scalefactor{scalefactor},
	blendtable{blendtable},
	textures{textures},
	layers{layers} {}

float TerrainInfo::get_scalefactor() const {
	return this->scalefactor;
}

size_t TerrainInfo::get_texture_count() const {
	return this->textures.size();
}

const Texture2dInfo &TerrainInfo::get_texture(size_t idx) const {
	return this->textures[idx];
}

const std::shared_ptr<BlendTableInfo> &TerrainInfo::get_blendtable() const {
	return this->blendtable;
}

size_t TerrainInfo::get_layer_count() const {
	return this->layers.size();
}

const TerrainLayerInfo &TerrainInfo::get_layer(size_t idx) const {
	return this->layers[idx];
}

} // namespace openage::renderer::resources
