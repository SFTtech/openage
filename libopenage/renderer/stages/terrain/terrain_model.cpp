// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#include "terrain_model.h"

#include "renderer/camera/camera.h"
#include "renderer/resources/assets/asset_manager.h"
#include "renderer/resources/assets/texture_manager.h"
#include "renderer/stages/terrain/terrain_mesh.h"
#include "renderer/stages/terrain/terrain_render_entity.h"
#include "renderer/uniform_input.h"

namespace openage::renderer::terrain {

TerrainRenderModel::TerrainRenderModel(const std::shared_ptr<renderer::resources::AssetManager> &asset_manager) :
	meshes{},
	camera{nullptr},
	asset_manager{asset_manager},
	render_entity{nullptr} {
}

void TerrainRenderModel::set_render_entity(const std::shared_ptr<TerrainRenderEntity> &entity) {
	this->render_entity = entity;
	this->update();
}

void TerrainRenderModel::set_camera(const std::shared_ptr<renderer::camera::Camera> &camera) {
	this->camera = camera;
}

void TerrainRenderModel::update() {
	// Check render entity for updates
	if (this->render_entity->is_changed()) {
		// TODO: Multiple meshes
		auto new_mesh = this->create_mesh();
		this->meshes.clear();
		this->meshes.push_back(new_mesh);

		// Indicate to the render entity that its updates have been processed.
		this->render_entity->clear_changed_flag();
	}

	// update uniforms
	for (auto mesh : this->meshes) {
		auto unifs = mesh->get_uniforms();
		if (unifs != nullptr) [[likely]] {
			// camera changes
			unifs->update(
				"view",
				this->camera->get_view_matrix(),
				"proj",
				this->camera->get_projection_matrix());

			if (mesh->is_changed()) {
				// mesh changes
				unifs->update(
					"model",
					mesh->get_model_matrix(),
					"tex",
					mesh->get_texture());
			}
		}
	}
}

const std::vector<std::shared_ptr<TerrainRenderMesh>> &TerrainRenderModel::get_meshes() const {
	return this->meshes;
}

std::shared_ptr<TerrainRenderMesh> TerrainRenderModel::create_mesh() {
	// Update mesh
	auto size = this->render_entity->get_size();
	auto src_verts = this->render_entity->get_vertices();

	// dst_verts places vertices in order
	// (left to right, bottom to top)
	std::vector<float> dst_verts{};
	dst_verts.reserve(src_verts.size() * 5);
	for (auto v : src_verts) {
		// Transform to scene coords
		dst_verts.push_back(-v.y);
		dst_verts.push_back(v.height);
		dst_verts.push_back(v.x);
		// TODO: Texture scaling
		dst_verts.push_back(v.x / 10);
		dst_verts.push_back(v.y / 10);
	}

	// split the grid into triangles using an index array
	std::vector<uint16_t> idxs;
	idxs.reserve((size[0] - 1) * (size[1] - 1) * 6);
	for (size_t i = 0; i < size[1] - 1; ++i) {
		for (size_t j = 0; j < size[0] - 1; ++j) {
			// since we are working on tiles, we split each tile into two triangles
			// with counter-clockwise vertex order
			idxs.push_back(i * size[0] + j); // bottom left
			idxs.push_back(i * size[0] + j + 1); // bottom right
			idxs.push_back(i * size[0] + j + size[1]); // top left
			idxs.push_back(i * size[0] + j + 1); // bottom right
			idxs.push_back(i * size[0] + j + size[1] + 1); // top right
			idxs.push_back(i * size[0] + j + size[1]); // top left
		}
	}

	resources::VertexInputInfo info{
		{resources::vertex_input_t::V3F32, resources::vertex_input_t::V2F32},
		resources::vertex_layout_t::AOS,
		resources::vertex_primitive_t::TRIANGLES,
		resources::index_t::U16};

	auto const vert_data_size = dst_verts.size() * sizeof(float);
	std::vector<uint8_t> vert_data(vert_data_size);
	std::memcpy(vert_data.data(), reinterpret_cast<const uint8_t *>(dst_verts.data()), vert_data_size);

	auto const idx_data_size = idxs.size() * sizeof(uint16_t);
	std::vector<uint8_t> idx_data(idx_data_size);
	std::memcpy(idx_data.data(), reinterpret_cast<const uint8_t *>(idxs.data()), idx_data_size);

	resources::MeshData meshdata{std::move(vert_data), std::move(idx_data), info};

	auto tex_manager = this->asset_manager->get_texture_manager();
	auto texture = tex_manager.request(this->render_entity->get_texture_path());

	auto terrain_mesh = std::make_shared<TerrainRenderMesh>(texture, std::move(meshdata));

	return terrain_mesh;
}

} // namespace openage::renderer::terrain
