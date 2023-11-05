// Copyright 2018-2023 the openage authors. See copying.md for legal info.

#include "terrain.h"

#include <algorithm>
#include <array>
#include <cstddef>

#include "gamestate/terrain_chunk.h"
#include "renderer/stages/terrain/terrain_render_entity.h"

namespace openage::gamestate {

Terrain::Terrain(const std::string &texture_path) :
	size{0, 0},
	height_map{},
	texture_path{texture_path},
	render_entity{nullptr} {
	// TODO: Actual terrain generation code
	this->size = util::Vector2s{10, 10};

	// fill the terrain grid with height values
	this->height_map.reserve(this->size[0] * this->size[1]);
	for (size_t i = 0; i < this->size[0] * this->size[1]; ++i) {
		this->height_map.push_back(0.0f);
	}
}

void Terrain::render_update() {
	if (this->render_entity != nullptr) {
		this->render_entity->update(this->size,
		                            this->height_map,
		                            this->texture_path);
	}
}

void Terrain::set_render_entity(const std::shared_ptr<renderer::terrain::TerrainRenderEntity> &entity) {
	this->render_entity = entity;

	this->render_update();
}

const std::vector<std::shared_ptr<TerrainChunk>> &Terrain::get_chunks() const {
	return this->chunks;
}

void Terrain::generate() {
	auto chunk = std::make_shared<TerrainChunk>(util::Vector2s{10, 10}, util::Vector2s{0, 0});
	chunk->set_render_entity(this->render_entity);
	chunk->render_update(time::time_t::zero(), this->texture_path);

	this->chunks.push_back(chunk);
}

} // namespace openage::gamestate
