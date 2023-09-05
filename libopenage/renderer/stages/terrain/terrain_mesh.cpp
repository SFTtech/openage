// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#include "terrain_mesh.h"

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
	terrain_info{nullptr, 0},
	uniforms{nullptr},
	mesh{renderer::resources::MeshData::make_quad()} {
}

TerrainRenderMesh::TerrainRenderMesh(const std::shared_ptr<renderer::resources::AssetManager> &asset_manager,
                                     const curve::Discrete<std::string> &terrain_path,
                                     renderer::resources::MeshData &&mesh) :
	require_renderable{true},
	changed{true},
	asset_manager{asset_manager},
	terrain_info{nullptr, 0},
	uniforms{nullptr},
	mesh{std::move(mesh)} {
	this->set_terrain_path(terrain_path);
}

void TerrainRenderMesh::set_mesh(renderer::resources::MeshData &&mesh) {
	this->mesh = mesh;
	this->require_renderable = true;
	this->changed = true;
}

const renderer::resources::MeshData &TerrainRenderMesh::get_mesh() {
	return this->mesh;
}

void TerrainRenderMesh::set_terrain_path(const curve::Discrete<std::string> &terrain_path) {
	this->changed = true;
	this->terrain_info.sync(terrain_path,
	                        std::function<std::shared_ptr<renderer::resources::TerrainInfo>(const std::string &)>(
								[&](const std::string &path) {
									if (path.empty()) {
										return std::shared_ptr<renderer::resources::TerrainInfo>{nullptr};
									}
									return this->asset_manager->request_terrain(path);
								}));
}

void TerrainRenderMesh::update_uniforms(const time::time_t &time) {
	// TODO: Only update uniforms that changed since last update
	if (this->uniforms == nullptr) [[unlikely]] {
		return;
	}

	if (not this->is_changed()) [[likely]] {
		return;
	}

	// local space -> world space
	this->uniforms->update("model", this->get_model_matrix());

	auto tex_info = this->terrain_info.get(time)->get_texture(0);
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

Eigen::Matrix4f TerrainRenderMesh::get_model_matrix() {
	// TODO: Needs input from engine
	auto transform = Eigen::Affine3f::Identity();
	return transform.matrix();
}

bool TerrainRenderMesh::is_changed() {
	return this->changed;
}

void TerrainRenderMesh::clear_changed_flag() {
	this->changed = false;
}

} // namespace openage::renderer::terrain
