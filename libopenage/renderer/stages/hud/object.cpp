// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#include "object.h"

#include "renderer/geometry.h"
#include "renderer/stages/hud/render_entity.h"


namespace openage::renderer::hud {

HudDragObject::HudDragObject(const std::shared_ptr<renderer::resources::AssetManager> &asset_manager) :
	require_renderable{true},
	changed{false},
	camera{nullptr},
	asset_manager{asset_manager},
	render_entity{nullptr},
	drag_pos{nullptr, 0, "", nullptr, {0, 0}},
	drag_start{0, 0},
	uniforms{nullptr},
	geometry{nullptr},
	last_update{0.0} {
}

void HudDragObject::set_render_entity(const std::shared_ptr<DragRenderEntity> &entity) {
	this->render_entity = entity;
	this->fetch_updates();
}

void HudDragObject::set_camera(const std::shared_ptr<renderer::camera::Camera> &camera) {
	this->camera = camera;
}

void HudDragObject::fetch_updates(const time::time_t &time) {
	if (not this->render_entity->is_changed()) {
		// exit early because there is nothing to do
		return;
	}

	// Get data from render entity
	this->drag_start = this->render_entity->get_drag_start();

	// Thread-safe access to curves needs a lock on the render entity's mutex
	auto read_lock = this->render_entity->get_read_lock();

	this->drag_pos.sync(this->render_entity->get_drag_pos() /* , this->last_update */);

	// Unlock the render entity mutex
	read_lock.unlock();

	// Set self to changed so that world renderer can update the renderable
	this->changed = true;
	this->render_entity->clear_changed_flag();
	this->last_update = time;
}

void HudDragObject::update_uniforms(const time::time_t & /* time */) {
	// TODO: Only update uniforms that changed since last update
	if (this->uniforms == nullptr) [[unlikely]] {
		return;
	}

	// TODO: Do something with the uniforms
}

void HudDragObject::update_geometry(const time::time_t &time) {
	// TODO: Only update geometry that changed since last update
	if (this->geometry == nullptr) [[unlikely]] {
		return;
	}

	auto drag_start_ndc = this->drag_start.to_viewport(this->camera).to_ndc_space(this->camera);
	auto drag_pos_ndc = this->drag_pos.get(time).to_viewport(this->camera).to_ndc_space(this->camera);

	float top = std::max(drag_start_ndc.y(), drag_pos_ndc.y());
	float bottom = std::min(drag_start_ndc.y(), drag_pos_ndc.y());
	float left = std::min(drag_start_ndc.x(), drag_pos_ndc.x());
	float right = std::max(drag_start_ndc.x(), drag_pos_ndc.x());

	log::log(SPAM << "top: " << top
	              << ", bottom: " << bottom
	              << ", left: " << left
	              << ", right: " << right);

	std::array<float, 16> quad_vertices{
		left, top, 0.0f, 1.0f, // top left corner
		left,
		bottom,
		0.0f,
		0.0f, // bottom left corner
		right,
		top,
		1.0f,
		1.0f, // top right corner
		right,
		bottom,
		1.0f,
		0.0f // bottom right corner
	};

	std::vector<uint8_t> vertex_data(quad_vertices.size() * sizeof(float));
	std::memcpy(vertex_data.data(), quad_vertices.data(), vertex_data.size());

	this->geometry->update_verts(vertex_data);
}

bool HudDragObject::requires_renderable() {
	return this->require_renderable;
}

void HudDragObject::clear_requires_renderable() {
	this->require_renderable = false;
}

bool HudDragObject::is_changed() {
	return this->changed;
}

void HudDragObject::clear_changed_flag() {
	this->changed = false;
}

void HudDragObject::set_uniforms(const std::shared_ptr<renderer::UniformInput> &uniforms) {
	this->uniforms = uniforms;
}

void HudDragObject::set_geometry(const std::shared_ptr<renderer::Geometry> &geometry) {
	this->geometry = geometry;
}

} // namespace openage::renderer::hud
