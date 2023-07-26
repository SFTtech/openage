// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#include "terrain_render_entity.h"

#include <algorithm>
#include <array>
#include <mutex>


namespace openage::renderer::terrain {

TerrainRenderEntity::TerrainRenderEntity() :
	changed{false},
	size{0, 0},
	vertices{},
	terrain_path{nullptr, 0} {
}

void TerrainRenderEntity::update(util::Vector2s size,
                                 std::vector<float> height_map,
                                 const std::string terrain_path,
                                 const time::time_t time) {
	std::unique_lock lock{this->mutex};

	// increase by 1 in every dimension because height_map
	// size is number of tiles, but we want number of vertices
	util::Vector2i tile_size{size[0], size[1]};
	this->size = util::Vector2s{size[0] + 1, size[1] + 1};

	// transfer mesh
	auto vert_count = this->size[0] * this->size[1];
	this->vertices.clear();
	this->vertices.reserve(vert_count);
	for (int i = 0; i < (int)this->size[0]; ++i) {
		for (int j = 0; j < (int)this->size[1]; ++j) {
			// for each vertex, compare the surrounding tiles
			std::vector<float> surround{};
			if (j - 1 >= 0 and i - 1 >= 0) {
				surround.push_back(height_map[(i - 1) * size[1] + j - 1]);
			}
			if (j < tile_size[1] and i - 1 >= 0) {
				surround.push_back(height_map[(i - 1) * size[1] + j]);
			}
			if (j < tile_size[1] and i < tile_size[0]) {
				surround.push_back(height_map[i * size[1] + j]);
			}
			if (j - 1 >= 0 and i < tile_size[0]) {
				surround.push_back(height_map[i * size[1] + j - 1]);
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

	// set texture path
	this->terrain_path.set_last(time, terrain_path);

	this->changed = true;
}

const std::vector<coord::scene3> &TerrainRenderEntity::get_vertices() {
	std::shared_lock lock{this->mutex};

	return this->vertices;
}

const curve::Discrete<std::string> &TerrainRenderEntity::get_terrain_path() {
	std::shared_lock lock{this->mutex};

	return this->terrain_path;
}

const util::Vector2s &TerrainRenderEntity::get_size() {
	std::shared_lock lock{this->mutex};

	return this->size;
}

bool TerrainRenderEntity::is_changed() {
	std::shared_lock lock{this->mutex};

	return this->changed;
}

void TerrainRenderEntity::clear_changed_flag() {
	std::unique_lock lock{this->mutex};

	this->changed = false;
}

} // namespace openage::renderer::terrain
