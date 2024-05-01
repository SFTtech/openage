// Copyright 2022-2024 the openage authors. See copying.md for legal info.

#include "mesh.h"

#include <functional>
#include <optional>
#include <utility>

#include "renderer/resources/assets/asset_manager.h"
#include "renderer/resources/assets/texture_manager.h"
#include "renderer/resources/terrain/terrain_info.h"
#include "renderer/resources/texture_info.h"
#include "renderer/uniform_input.h"


namespace openage::renderer::terrain {


TerrainRenderMesh::TerrainRenderMesh(const std::shared_ptr<renderer::resources::AssetManager> &asset_manager) :
	require_renderable{false},
	changed{false},
	asset_manager{asset_manager},
	terrain_info{nullptr},
	uniforms{nullptr},
	mesh{renderer::resources::MeshData::make_quad()} {
}

TerrainRenderMesh::TerrainRenderMesh(const std::shared_ptr<renderer::resources::AssetManager> &asset_manager,
                                     const std::shared_ptr<renderer::resources::TerrainInfo> &info,
                                     renderer::resources::MeshData &&mesh) :
	require_renderable{true},
	changed{true},
	asset_manager{asset_manager},
	terrain_info{nullptr},
	uniforms{nullptr},
	mesh{std::move(mesh)} {
	this->set_terrain_info(info);
}

void TerrainRenderMesh::set_mesh(renderer::resources::MeshData &&mesh) {
	this->mesh = mesh;
	this->require_renderable = true;
	this->changed = true;
}

const renderer::resources::MeshData &TerrainRenderMesh::get_mesh() {
	return this->mesh;
}

void TerrainRenderMesh::set_terrain_info(const std::shared_ptr<renderer::resources::TerrainInfo> &info) {
	this->changed = true;
	this->terrain_info = info;
}

void TerrainRenderMesh::update_uniforms(const time::time_t & /* time */) {
	// TODO: Only update uniforms that changed since last update
	if (this->uniforms == nullptr) [[unlikely]] {
		return;
	}

	if (not this->is_changed()) [[likely]] {
		return;
	}

	// local space -> world space
	this->uniforms->update("model", this->model_matrix);

	auto tex_info = this->terrain_info->get_texture(0);
	auto tex_manager = this->asset_manager->get_texture_manager();
	auto texture = tex_manager->request(tex_info->get_image_path().value());

	this->uniforms->update("tex", texture);

	this->changed = false;
}

bool TerrainRenderMesh::requires_renderable() {
	return this->require_renderable;
}

void TerrainRenderMesh::clear_requires_renderable() {
	this->require_renderable = false;
}

void TerrainRenderMesh::set_uniforms(const std::shared_ptr<renderer::UniformInput> &uniforms) {
	this->uniforms = uniforms;
}

const std::shared_ptr<renderer::UniformInput> &TerrainRenderMesh::get_uniforms() {
	return this->uniforms;
}

void TerrainRenderMesh::create_model_matrix(const coord::scene2_delta &offset) {
	// TODO: Needs input from engine
	auto model = Eigen::Affine3f::Identity();
	model.translate(offset.to_world_space());
	this->model_matrix = model.matrix();
}

bool TerrainRenderMesh::is_changed() {
	return this->changed;
}

void TerrainRenderMesh::clear_changed_flag() {
	this->changed = false;
}

} // namespace openage::renderer::terrain
