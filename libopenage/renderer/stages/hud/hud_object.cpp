// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "hud_object.h"

#include "renderer/stages/hud/hud_render_entity.h"


namespace openage::renderer::hud {

HudObject::HudObject(const std::shared_ptr<renderer::resources::AssetManager> &asset_manager) :
	require_renderable{true},
	changed{false},
	camera{nullptr},
	asset_manager{asset_manager},
	render_entity{nullptr},
	uniforms{nullptr},
	last_update{0.0} {
}

void HudObject::set_render_entity(const std::shared_ptr<HudRenderEntity> &entity) {
	this->render_entity = entity;
	this->fetch_updates();
}

void HudObject::set_camera(const std::shared_ptr<renderer::camera::Camera> &camera) {
	this->camera = camera;
}

void HudObject::fetch_updates(const time::time_t &time) {
	if (not this->render_entity->is_changed()) {
		// exit early because there is nothing to do
		return;
	}
	// Get data from render entity
	// TODO

	// Set self to changed so that world renderer can update the renderable
	this->changed = true;
	this->render_entity->clear_changed_flag();
	this->last_update = time;
}

void HudObject::update_uniforms(const time::time_t &time) {
	// TODO: Only update uniforms that changed since last update
	if (this->uniforms == nullptr) [[unlikely]] {
		return;
	}
	// TODO
}

bool HudObject::requires_renderable() {
	return this->require_renderable;
}

void HudObject::clear_requires_renderable() {
	this->require_renderable = false;
}

bool HudObject::is_changed() {
	return this->changed;
}

void HudObject::clear_changed_flag() {
	this->changed = false;
}

void HudObject::set_uniforms(const std::shared_ptr<renderer::UniformInput> &uniforms) {
	this->uniforms = uniforms;
}

} // namespace openage::renderer::hud
