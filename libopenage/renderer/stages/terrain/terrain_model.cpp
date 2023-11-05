// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#include "terrain_model.h"

#include <array>
#include <cstdint>
#include <cstring>
#include <utility>

#include <eigen3/Eigen/Dense>

#include "coord/scene.h"
#include "renderer/resources/assets/asset_manager.h"
#include "renderer/resources/mesh_data.h"
#include "renderer/stages/terrain/terrain_chunk.h"
#include "renderer/stages/terrain/terrain_render_entity.h"
#include "util/fixed_point.h"
#include "util/vector.h"


namespace openage::renderer::terrain {

TerrainRenderModel::TerrainRenderModel(const std::shared_ptr<renderer::resources::AssetManager> &asset_manager) :
	chunks{},
	camera{nullptr},
	asset_manager{asset_manager} {
}

void TerrainRenderModel::set_render_entity(const std::shared_ptr<TerrainRenderEntity> &entity) {
	// ASDF: Set chunk size and offset from parameters
	auto chunk = std::make_shared<TerrainChunk>(this->asset_manager, util::Vector2s{10, 10}, util::Vector2s{0, 0});
	chunk->set_render_entity(entity);
	chunk->fetch_updates();

	this->chunks.push_back(chunk);
}

void TerrainRenderModel::set_camera(const std::shared_ptr<renderer::camera::Camera> &camera) {
	this->camera = camera;
}

void TerrainRenderModel::fetch_updates(const time::time_t &time) {
	for (auto &chunk : this->chunks) {
		chunk->fetch_updates(time);
	}
}

void TerrainRenderModel::update_uniforms(const time::time_t &time) {
	for (auto &chunk : this->chunks) {
		chunk->update_uniforms(time);
	}
}

const std::vector<std::shared_ptr<TerrainChunk>> &TerrainRenderModel::get_chunks() const {
	return this->chunks;
}

} // namespace openage::renderer::terrain
