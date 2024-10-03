// Copyright 2022-2024 the openage authors. See copying.md for legal info.

#include "render_entity.h"

#include <algorithm>
#include <array>
#include <mutex>


namespace openage::renderer::terrain {

RenderEntity::RenderEntity() :
	renderer::RenderEntity{},
	size{0, 0},
	tiles{},
	terrain_paths{},
	vertices{} {
}

void RenderEntity::update_tile(const util::Vector2s size,
                               const coord::tile &pos,
                               const terrain_elevation_t elevation,
                               const std::string terrain_path,
                               const time::time_t time) {
	std::unique_lock lock{this->mutex};

	if (this->vertices.empty()) {
		throw Error(MSG(err) << "Cannot update tile: Vertices have not been initialized yet.");
	}

	// find the postion of the tile in the vertex array
	auto left_corner = pos.ne * size[0] + pos.se;

	// update the 4 vertices of the tile
	this->vertices[left_corner].up = elevation.to_float();                 // left corner
	this->vertices[left_corner + 1].up = elevation.to_float();             // bottom corner
	this->vertices[left_corner + (size[0] + 1)].up = elevation.to_float(); // top corner
	this->vertices[left_corner + (size[0] + 2)].up = elevation.to_float(); // right corner

	// update tile
	this->tiles[left_corner] = {elevation, terrain_path};

	// update the last update time
	this->last_update = time;

	// update the terrain paths
	this->terrain_paths.insert(terrain_path);

	this->changed = true;
}

void RenderEntity::update(const util::Vector2s size,
                          const tiles_t tiles,
                          const time::time_t time) {
	std::unique_lock lock{this->mutex};

	// increase by 1 in every dimension because tiles
	// size is number of tiles, but we want number of vertices
	util::Vector2i tile_size{size[0], size[1]};
	this->size = util::Vector2s{size[0] + 1, size[1] + 1};

	// transfer mesh
	auto vert_count = this->size[0] * this->size[1];
	this->vertices.clear();
	this->vertices.reserve(vert_count);
	for (int j = 0; j < (int)this->size[1]; ++j) {
		for (int i = 0; i < (int)this->size[0]; ++i) {
			// for each vertex, compare the surrounding tiles
			std::vector<float> surround{};
			if (j - 1 >= 0 and i - 1 >= 0) {
				surround.push_back(tiles[(i - 1) * size[1] + j - 1].first.to_float());
			}
			if (j < tile_size[1] and i - 1 >= 0) {
				surround.push_back(tiles[(i - 1) * size[1] + j].first.to_float());
			}
			if (j < tile_size[1] and i < tile_size[0]) {
				surround.push_back(tiles[i * size[1] + j].first.to_float());
			}
			if (j - 1 >= 0 and i < tile_size[0]) {
				surround.push_back(tiles[i * size[1] + j - 1].first.to_float());
			}
			// select the height of the highest surrounding tile
			auto max_height = *std::max_element(surround.begin(), surround.end());
			coord::scene3 v{
				static_cast<float>(i),
				static_cast<float>(j),
				max_height,
			};
			this->vertices.push_back(v);
		}
	}

	// update tiles
	this->tiles = tiles;

	// update the last update time
	this->last_update = time;

	// update the terrain paths
	this->terrain_paths.clear();
	for (const auto &tile : this->tiles) {
		this->terrain_paths.insert(tile.second);
	}

	this->changed = true;
}

const std::vector<coord::scene3> RenderEntity::get_vertices() {
	std::shared_lock lock{this->mutex};

	return this->vertices;
}

const RenderEntity::tiles_t RenderEntity::get_tiles() {
	std::shared_lock lock{this->mutex};

	return this->tiles;
}

const std::unordered_set<std::string> RenderEntity::get_terrain_paths() {
	std::shared_lock lock{this->mutex};

	return this->terrain_paths;
}

const util::Vector2s RenderEntity::get_size() {
	std::shared_lock lock{this->mutex};

	return this->size;
}

} // namespace openage::renderer::terrain
