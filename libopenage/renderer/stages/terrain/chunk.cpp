// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#include "chunk.h"

#include "renderer/resources/assets/asset_manager.h"
#include "renderer/resources/mesh_data.h"
#include "renderer/stages/terrain/mesh.h"
#include "renderer/stages/terrain/render_entity.h"


namespace openage::renderer::terrain {

TerrainChunk::TerrainChunk(const std::shared_ptr<renderer::resources::AssetManager> &asset_manager,
                           const util::Vector2s size,
                           const coord::scene2_delta offset) :
	size{size},
	offset{offset},
	asset_manager{asset_manager} {}

void TerrainChunk::set_render_entity(const std::shared_ptr<RenderEntity> &entity) {
	this->render_entity = entity;
}

void TerrainChunk::fetch_updates(const time::time_t & /* time */) {
	// TODO: Don't create model if render entity is not set
	if (not this->render_entity) {
		return;
	}

	// Check render entity for updates
	if (not this->render_entity->is_changed()) {
		return;
	}

	// Get the terrain data from the render entity
	auto terrain_size = this->render_entity->get_size();
	auto terrain_paths = this->render_entity->get_terrain_paths();
	auto tiles = this->render_entity->get_tiles();
	auto heightmap_verts = this->render_entity->get_vertices();

	// Recreate the mesh data
	// TODO: Change mesh instead of recreating it
	// TODO: Multiple meshes
	this->meshes.clear();
	for (const auto &terrain_path : terrain_paths) {
		auto new_mesh = this->create_mesh(terrain_size, tiles, heightmap_verts, terrain_path);
		new_mesh->create_model_matrix(this->offset);
		this->meshes.push_back(new_mesh);
	}

	// auto new_mesh = this->create_mesh();
	// new_mesh->create_model_matrix(this->offset);
	// this->meshes.clear();
	// this->meshes.push_back(new_mesh);

	// Indicate to the render entity that its updates have been processed.
	this->render_entity->clear_changed_flag();
}

void TerrainChunk::update_uniforms(const time::time_t &time) {
	for (auto &mesh : this->meshes) {
		mesh->update_uniforms(time);
	}
}

const std::vector<std::shared_ptr<TerrainRenderMesh>> &TerrainChunk::get_meshes() const {
	return this->meshes;
}

std::shared_ptr<TerrainRenderMesh> TerrainChunk::create_mesh(const util::Vector2s vert_size,
                                                             const RenderEntity::tiles_t &tiles,
                                                             const std::vector<coord::scene3> &heightmap_verts,
                                                             const std::string &texture_path) {
	auto v_width = vert_size[0];
	auto v_height = vert_size[1];

	// vertex data for the mesh
	std::vector<float> mesh_verts{};

	// vertex indices for the mesh
	std::vector<uint16_t> idxs{};

	// maps indices of verts in the heightmap to indices in the vertex data vector
	std::unordered_map<size_t, size_t> index_map;

	for (size_t i = 0; i < v_width - 1; ++i) {
		for (size_t j = 0; j < v_height - 1; ++j) {
			auto tile = tiles.at(j + i * (v_height - 1));
			if (tile.second != texture_path) {
				// Skip tiles with different textures
				continue;
			}

			// indices of the vertices of the current tile
			// in the hightmap
			std::array<size_t, 4> tile_verts{
				j + i * v_height,           // top left
				j + (i + 1) * v_height,     // bottom left
				j + 1 + (i + 1) * v_height, // bottom right
				j + 1 + i * v_height,       // top right
			};

			// add the vertices of the current tile to the vertex data vector
			for (size_t v_idx : tile_verts) {
				// skip if the vertex is already in the vertex data vector
				if (not index_map.contains(v_idx)) {
					auto v = heightmap_verts[v_idx];
					auto v_vec = v.to_world_space();
					mesh_verts.push_back(v_vec[0]);
					mesh_verts.push_back(v_vec[1]);
					mesh_verts.push_back(v_vec[2]);
					mesh_verts.push_back((v.ne / 10).to_float());
					mesh_verts.push_back((v.se / 10).to_float());

					// update the index map
					// since new verts are added to the end of the vertex data vector
					// the mapped index is the current size of the index map
					index_map[v_idx] = index_map.size();
				}
			}

			// first triangle
			idxs.push_back(index_map[tile_verts[0]]); // top left
			idxs.push_back(index_map[tile_verts[1]]); // bottom left
			idxs.push_back(index_map[tile_verts[2]]); // bottom right

			// second triangle
			idxs.push_back(index_map[tile_verts[0]]); // top left
			idxs.push_back(index_map[tile_verts[2]]); // bottom right
			idxs.push_back(index_map[tile_verts[3]]); // top right
		}
	}

	resources::VertexInputInfo info{
		{resources::vertex_input_t::V3F32, resources::vertex_input_t::V2F32},
		resources::vertex_layout_t::AOS,
		resources::vertex_primitive_t::TRIANGLES,
		resources::index_t::U16};

	auto const vert_data_size_new = mesh_verts.size() * sizeof(float);
	std::vector<uint8_t> vert_data_new(vert_data_size_new);
	std::memcpy(vert_data_new.data(), mesh_verts.data(), vert_data_size_new);

	auto const idx_data_size = idxs.size() * sizeof(uint16_t);
	std::vector<uint8_t> idx_data(idx_data_size);
	std::memcpy(idx_data.data(), idxs.data(), idx_data_size);

	resources::MeshData meshdata{std::move(vert_data_new), std::move(idx_data), info};

	// Create the terrain mesh
	auto terrain_info = this->asset_manager->request_terrain(texture_path);
	auto terrain_mesh = std::make_shared<TerrainRenderMesh>(
		this->asset_manager,
		terrain_info,
		std::move(meshdata));

	return terrain_mesh;
}

util::Vector2s &TerrainChunk::get_size() {
	return this->size;
}

coord::scene2_delta &TerrainChunk::get_offset() {
	return this->offset;
}

} // namespace openage::renderer::terrain
