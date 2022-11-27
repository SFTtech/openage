// Copyright 2022-2022 the openage authors. See copying.md for legal info.

#include "terrain_mesh.h"

#include "renderer/resources/assets/texture_manager.h"
#include "renderer/resources/mesh_data.h"
#include "renderer/resources/texture_data.h"
#include "renderer/stages/terrain/terrain_render_entity.h"

namespace openage::renderer::terrain {

TerrainRenderMesh::TerrainRenderMesh(const std::shared_ptr<renderer::Renderer> &renderer,
                                     const std::shared_ptr<TerrainRenderEntity> &entity) :
	texture_manager{std::make_shared<renderer::resources::TextureManager>(renderer)},
	render_entity{entity},
	texture{},
	mesh{renderer::resources::MeshData::make_quad()} {
}

void TerrainRenderMesh::set_render_entity(const std::shared_ptr<TerrainRenderEntity> &entity) {
	this->render_entity = entity;
	this->update();
}

void TerrainRenderMesh::update() {
	if (not this->render_entity->is_changed()) {
		return;
	}

	// Update mesh
	auto size = this->render_entity->get_size();
	auto src_verts = this->render_entity->get_vertices();

	// dst_verts places vertices in order
	// (left to right, bottom to top)
	std::vector<float> dst_verts{};
	dst_verts.reserve(src_verts.size() * 3);
	for (auto v : src_verts) {
		dst_verts.push_back(v.x);
		dst_verts.push_back(v.y);
		dst_verts.push_back(v.height);
	}

	// split the grid into triangles
	// with an index array
	std::vector<uint16_t> idxs;
	for (size_t i = 0; i < size[1] - 1; ++i) {
		for (size_t j = 0; j < size[0] - 1; ++j) {
			// since we are working on tiles, we split each tile into two triangles
			// with counter-clockwise vertex order
			idxs.push_back(i * size[0] + j); // bottom left
			idxs.push_back(i * size[0] + j + 1); // bottom right
			idxs.push_back(i * size[0] + j + size[0]); // top left
			idxs.push_back(i * size[0] + j + 1); // bottom right
			idxs.push_back(i * size[0] + j + size[0] + 1); // top right
			idxs.push_back(i * size[0] + j + size[0]); // top left
		}
	}

	resources::VertexInputInfo info{
		{resources::vertex_input_t::V2F32, resources::vertex_input_t::V2F32},
		resources::vertex_layout_t::AOS,
		resources::vertex_primitive_t::TRIANGLES,
		resources::index_t::U16};

	auto const vert_data_size = dst_verts.size() * sizeof(float);
	std::vector<uint8_t> vert_data(vert_data_size);
	std::memcpy(vert_data.data(), reinterpret_cast<const uint8_t *>(dst_verts.data()), vert_data_size);

	auto const idx_data_size = idxs.size() * sizeof(uint16_t);
	std::vector<uint8_t> idx_data(idx_data_size);
	std::memcpy(idx_data.data(), reinterpret_cast<const uint8_t *>(idxs.data()), idx_data_size);

	resources::MeshData new_mesh{std::move(vert_data), std::move(idx_data), info};
	this->mesh = new_mesh;

	// ASDF: testing
	this->mesh = resources::MeshData::make_quad();

	// Update textures
	this->texture = this->texture_manager->request(this->render_entity->get_texture_path());

	// Indicate to the render entity that its updates have been processed.
	this->render_entity->clear_changed_flag();
}

const renderer::resources::MeshData &TerrainRenderMesh::get_mesh() {
	return this->mesh;
}

const std::shared_ptr<renderer::Texture2d> &TerrainRenderMesh::get_texture() {
	return this->texture;
}

} // namespace openage::renderer::terrain