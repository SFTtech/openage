// Copyright 2022-2024 the openage authors. See copying.md for legal info.

#include "model.h"

#include <array>
#include <cstdint>
#include <cstring>
#include <utility>

#include <eigen3/Eigen/Dense>

#include "coord/scene.h"
#include "renderer/resources/assets/asset_manager.h"
#include "renderer/resources/mesh_data.h"
#include "renderer/stages/terrain/chunk.h"
#include "renderer/stages/terrain/render_entity.h"
#include "util/fixed_point.h"
#include "util/vector.h"


namespace openage::renderer::terrain {

TerrainRenderModel::TerrainRenderModel(const std::shared_ptr<renderer::resources::AssetManager> &asset_manager) :
	chunks{},
	camera{nullptr},
	asset_manager{asset_manager} {
}

void TerrainRenderModel::add_chunk(const std::shared_ptr<RenderEntity> &entity,
                                   const util::Vector2s size,
                                   const coord::scene2_delta offset) {
	auto chunk = std::make_shared<TerrainChunk>(this->asset_manager, size, offset);
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
