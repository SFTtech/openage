// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#include "terrain_mesh.h"

namespace openage::renderer::terrain {


TerrainRenderMesh::TerrainRenderMesh() :
	require_renderable{false},
	changed{false},
	texture{nullptr},
	uniforms{nullptr},
	mesh{renderer::resources::MeshData::make_quad()} {
}

TerrainRenderMesh::TerrainRenderMesh(const std::shared_ptr<renderer::Texture2d> &texture,
                                     renderer::resources::MeshData &&mesh) :
	require_renderable{true},
	changed{false},
	uniforms{nullptr},
	mesh{std::move(mesh)} {
	this->set_texture(texture);
}

void TerrainRenderMesh::set_mesh(renderer::resources::MeshData &&mesh) {
	this->mesh = mesh;
	this->require_renderable = true;
}

const renderer::resources::MeshData &TerrainRenderMesh::get_mesh() {
	return this->mesh;
}

void TerrainRenderMesh::set_texture(const std::shared_ptr<renderer::Texture2d> &texture) {
	this->texture = texture;
	this->changed = true;
}

const std::shared_ptr<renderer::Texture2d> &TerrainRenderMesh::get_texture() {
	return this->texture;
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
