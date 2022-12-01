// Copyright 2022-2022 the openage authors. See copying.md for legal info.

#include "world_object.h"

#include "renderer/resources/assets/texture_manager.h"
#include "renderer/resources/mesh_data.h"
#include "renderer/resources/texture_data.h"
#include "renderer/stages/world/world_render_entity.h"
#include "renderer/uniform_input.h"

namespace openage::renderer::world {

WorldObject::WorldObject() :
	require_renderable{true},
	changed{false},
	texture_manager{nullptr},
	render_entity{nullptr},
	position{0.0f, 0.0f, 0.0f},
	texture{nullptr},
	uniforms{nullptr} {
}

void WorldObject::set_render_entity(const std::shared_ptr<WorldRenderEntity> &entity) {
	this->render_entity = entity;
	this->update();
}

void WorldObject::update() {
	if (not this->render_entity->is_changed()) {
		return;
	}

	// TODO: Get position

	// ASDF: New renderable is only required if the mesh is changed
	this->require_renderable = true;

	// Update textures
	this->texture = this->texture_manager->request(this->render_entity->get_texture_path());
	if (this->uniforms != nullptr) {
		this->uniforms->update("tex", this->texture);
	}

	this->changed = true;

	// Indicate to the render entity that its updates have been processed.
	this->render_entity->clear_changed_flag();
}

const renderer::resources::MeshData WorldObject::get_mesh() {
	return resources::MeshData::make_quad();
}

const std::shared_ptr<renderer::Texture2d> &WorldObject::get_texture() {
	return this->texture;
}

bool WorldObject::requires_renderable() {
	return this->require_renderable;
}

void WorldObject::clear_requires_renderable() {
	this->require_renderable = false;
}

bool WorldObject::is_changed() {
	return this->changed;
}

void WorldObject::clear_changed_flag() {
	this->changed = false;
}

void WorldObject::set_uniforms(const std::shared_ptr<renderer::UniformInput> &uniforms) {
	this->uniforms = uniforms;
}

} // namespace openage::renderer::world