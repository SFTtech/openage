// Copyright 2018-2023 the openage authors. See copying.md for legal info.

#include "terrain_chunk.h"


namespace openage::gamestate {

TerrainChunk::TerrainChunk(const util::Vector2s size,
                           const util::Vector2s offset) :
	size{size},
	offset{offset},
	height_map{} {
	if (this->size[0] > MAX_CHUNK_WIDTH || this->size[1] > MAX_CHUNK_HEIGHT) {
		throw Error(MSG(err) << "Terrain chunk size exceeds maximum size: "
		                     << this->size[0] << "x" << this->size[1] << " > "
		                     << MAX_CHUNK_WIDTH << "x" << MAX_CHUNK_HEIGHT);
	}

	// fill the terrain grid with height values
	this->height_map.reserve(this->size[0] * this->size[1]);
	for (size_t i = 0; i < this->size[0] * this->size[1]; ++i) {
		this->height_map.push_back(0.0f);
	}
}

void TerrainChunk::set_render_entity(const std::shared_ptr<renderer::terrain::TerrainRenderEntity> &entity) {
	this->render_entity = entity;
}

void TerrainChunk::render_update(const time::time_t &time,
                                 const std::string &terrain_path) {
	if (this->render_entity != nullptr) {
		this->render_entity->update(this->size,
		                            this->height_map,
		                            terrain_path,
		                            time);
	}
}

const util::Vector2s &TerrainChunk::get_size() const {
	return this->size;
}

const util::Vector2s &TerrainChunk::get_offset() const {
	return this->offset;
}

} // namespace openage::gamestate
