// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#include "world_object.h"

#include <array>
#include <cstddef>
#include <functional>
#include <optional>
#include <utility>

#include <eigen3/Eigen/Dense>

#include "renderer/camera/camera.h"
#include "renderer/definitions.h"
#include "renderer/resources/animation/angle_info.h"
#include "renderer/resources/animation/animation_info.h"
#include "renderer/resources/animation/frame_info.h"
#include "renderer/resources/animation/layer_info.h"
#include "renderer/resources/assets/asset_manager.h"
#include "renderer/resources/assets/texture_manager.h"
#include "renderer/resources/frame_timing.h"
#include "renderer/resources/mesh_data.h"
#include "renderer/resources/texture_info.h"
#include "renderer/resources/texture_subinfo.h"
#include "renderer/stages/world/world_render_entity.h"
#include "renderer/uniform_input.h"
#include "util/fixed_point.h"
#include "util/vector.h"


namespace openage::renderer::world {

WorldObject::WorldObject(const std::shared_ptr<renderer::resources::AssetManager> &asset_manager) :
	require_renderable{true},
	changed{false},
	camera{nullptr},
	asset_manager{asset_manager},
	render_entity{nullptr},
	ref_id{0},
	position{nullptr, 0, "", nullptr, SCENE_ORIGIN},
	angle{nullptr, 0, "", nullptr, 0},
	animation_info{nullptr, 0},
	uniforms{nullptr},
	last_update{0.0} {
}

void WorldObject::set_render_entity(const std::shared_ptr<WorldRenderEntity> &entity) {
	this->render_entity = entity;
	this->fetch_updates();
}

void WorldObject::set_camera(const std::shared_ptr<renderer::camera::Camera> &camera) {
	this->camera = camera;
}

void WorldObject::fetch_updates(const time::time_t &time) {
	if (not this->render_entity->is_changed()) {
		// exit early because there is nothing to do
		return;
	}
	// Get data from render entity
	this->ref_id = this->render_entity->get_id();
	this->position.sync(this->render_entity->get_position());
	this->animation_info.sync(this->render_entity->get_animation_path(),
	                          std::function<std::shared_ptr<renderer::resources::Animation2dInfo>(const std::string &)>(
								  [&](const std::string &path) {
									  if (path.empty()) {
										  auto placeholder = this->asset_manager->get_placeholder_animation();
										  if (placeholder) {
											  return (*placeholder).second;
										  }
										  return std::shared_ptr<renderer::resources::Animation2dInfo>{nullptr};
									  }
									  return this->asset_manager->request_animation(path);
								  }),
	                          this->last_update);
	this->angle.sync(this->render_entity->get_angle(), this->last_update);

	// Set self to changed so that world renderer can update the renderable
	this->changed = true;
	this->render_entity->clear_changed_flag();
	this->last_update = time;
}

void WorldObject::update_uniforms(const time::time_t &time) {
	// TODO: Only update uniforms that changed since last update
	if (this->uniforms == nullptr) [[unlikely]] {
		return;
	}

	// Object world position
	auto current_pos = this->position.get(time);
	this->uniforms->update(this->obj_world_position, current_pos.to_world_space());

	// Direction angle the object is facing towards currently
	auto angle_degrees = this->angle.get(time).to_float();

	// Frame subtexture
	auto animation_info = this->animation_info.get(time);
	auto &layer = animation_info->get_layer(0); // TODO: Support multiple layers
	auto &angle = layer.get_direction_angle(angle_degrees);

	// Flip subtexture horizontally if angle is mirrored
	if (angle->is_mirrored()) {
		this->uniforms->update(this->flip_x, true);
	}
	else {
		this->uniforms->update(this->flip_x, false);
	}

	// Current frame index considering current time
	size_t frame_idx;
	switch (layer.get_display_mode()) {
	case renderer::resources::display_mode::ONCE:
	case renderer::resources::display_mode::LOOP: {
		// ONCE and LOOP are animated based on time
		auto &timing = layer.get_frame_timing();
		frame_idx = timing->get_frame(time, this->render_entity->get_update_time());
	} break;
	case renderer::resources::display_mode::OFF:
	default:
		// OFF only shows the first frame
		frame_idx = 0;
		break;
	}

	// Index of texture and subtexture where the frame's pixels are located
	auto &frame_info = angle->get_frame(frame_idx);
	auto tex_idx = frame_info->get_texture_idx();
	auto subtex_idx = frame_info->get_subtexture_idx();

	auto &tex_info = animation_info->get_texture(tex_idx);
	auto &tex_manager = this->asset_manager->get_texture_manager();
	auto &texture = tex_manager->request(tex_info->get_image_path().value());
	this->uniforms->update(this->tex, texture);

	// Subtexture coordinates.inside texture
	auto coords = tex_info->get_subtex_info(subtex_idx).get_tile_params();
	this->uniforms->update(this->tile_params, coords);

	// scale and keep width x height ratio of texture
	// when the viewport size changes
	auto scale = animation_info->get_scalefactor() / this->camera->get_zoom();
	auto screen_size = this->camera->get_viewport_size();
	auto subtex_size = tex_info->get_subtex_info(subtex_idx).get_size();

	// Scaling with viewport size and zoom
	auto scale_vec = Eigen::Vector2f{
		scale * (static_cast<float>(subtex_size[0]) / screen_size[0]),
		scale * (static_cast<float>(subtex_size[1]) / screen_size[1])};
	this->uniforms->update(this->scale, scale_vec);

	// Move subtexture in scene so that its anchor point is at the object's position
	auto anchor = tex_info->get_subtex_info(subtex_idx).get_anchor_params();
	auto anchor_offset = Eigen::Vector2f{
		scale * (static_cast<float>(anchor[0]) / screen_size[0]),
		scale * (static_cast<float>(anchor[1]) / screen_size[1])};
	this->uniforms->update(this->anchor_offset, anchor_offset);
}

uint32_t WorldObject::get_id() {
	return this->ref_id;
}

const renderer::resources::MeshData WorldObject::get_mesh() {
	return resources::MeshData::make_quad();
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
