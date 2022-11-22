// Copyright 2018-2018 the openage authors. See copying.md for legal info.

#include "terrain.h"

#include "renderer/terrain/terrain_render_entity.h"

namespace openage::gamestate {

Terrain::Terrain(const std::shared_ptr<renderer::terrain::TerrainRenderEntity> &render_entity,
                 util::Path &texture_path) :
	size{0, 0},
	height_map{},
	texture_path{texture_path},
	render_entity{render_entity} {
	// ASDF: testing
	this->size = util::Vector2s{10, 10};

	// fill the terrain grid with height values
	this->height_map.reserve(this->size[0] * this->size[1]);
	for (size_t i = 0; i < this->size[0] * this->size[1]; ++i) {
		this->height_map.push_back(0.0f);
	}

	// Create "test bumps" in the terrain to check if rendering works
	this->height_map[11] = 1.0f;
	this->height_map[23] = 2.3f;
	this->height_map[42] = 4.2f;
	this->height_map[69] = 6.9f; // nice

	// A hill
	this->height_map[55] = 3.0f; // center
	this->height_map[45] = 2.0f; // bottom left slope
	this->height_map[35] = 1.0f;
	this->height_map[56] = 1.0f; // bottom right slope (little steeper)
	this->height_map[65] = 2.0f; // top right slope
	this->height_map[75] = 1.0f;
	this->height_map[54] = 2.0f; // top left slope
	this->height_map[53] = 1.0f;
}

void Terrain::push_to_render() {
	if (this->render_entity != nullptr) {
		this->render_entity->update();
	}
}

} // namespace openage::gamestate
