// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#include "world_object.h"

#include <eigen3/Eigen/Dense>

#include "renderer/camera/camera.h"
#include "renderer/resources/animation/angle_info.h"
#include "renderer/resources/animation/animation_info.h"
#include "renderer/resources/animation/frame_info.h"
#include "renderer/resources/assets/asset_manager.h"
#include "renderer/resources/assets/texture_manager.h"
#include "renderer/resources/mesh_data.h"
#include "renderer/resources/texture_data.h"
#include "renderer/stages/world/world_render_entity.h"
#include "renderer/uniform_input.h"

namespace openage::renderer::world {

WorldObject::WorldObject(const std::shared_ptr<renderer::resources::AssetManager> &asset_manager) :
	require_renderable{true},
	changed{false},
	camera{nullptr},
	asset_manager{asset_manager},
	render_entity{nullptr},
	ref_id{0},
	position{0.0f, 0.0f, 0.0f},
	texture{nullptr},
	uniforms{nullptr} {
}

void WorldObject::set_render_entity(const std::shared_ptr<WorldRenderEntity> &entity) {
	this->render_entity = entity;
	this->update();
}

void WorldObject::set_camera(const std::shared_ptr<renderer::camera::Camera> &camera) {
	this->camera = camera;
}

void WorldObject::update(const curve::time_t &time) {
	if (this->render_entity->is_changed()) {
		// Get ID
		this->ref_id = this->render_entity->get_id();

		// Get position
		this->position = this->render_entity->get_position();

		// TODO: New renderable is only required if the mesh is changed
		this->require_renderable = true;

		// Update textures
		auto anim_info = this->asset_manager->request_animation(this->render_entity->get_texture_path());
		auto tex_manager = this->asset_manager->get_texture_manager();
		this->texture = tex_manager.request(anim_info->get_texture(0)->get_image_path().value());
		// TODO: Support multiple textures per animation

		this->changed = true;

		// Indicate to the render entity that its updates have been processed.
		this->render_entity->clear_changed_flag();

		// Return to let the renderer create a new renderable
		return;
	}

	// Update uniforms if no render entity changes are detected
	this->update_uniforms(time);
}

void WorldObject::update_uniforms(const curve::time_t &time) {
	if (this->uniforms != nullptr) [[likely]] {
		/* Frame subtexture */
		auto anim_info = this->asset_manager->request_animation(this->render_entity->get_texture_path());
		auto layer = anim_info->get_layer(0); // TODO: Support multiple layers
		auto angle = layer.get_angle(0); // TODO: Support multiple angles

		// Current frame index considering current time
		auto timing = layer.get_frame_timing();
		size_t frame_idx = timing->get_mod(time, this->render_entity->get_update_time());

		// Get index of texture and subtexture where the frame's pixels are located
		auto frame_info = angle->get_frame(frame_idx);
		auto tex_idx = frame_info->get_texture_idx();
		auto subtex_idx = frame_info->get_subtexture_idx();

		// Get the texture info
		auto tex_info = anim_info->get_texture(tex_idx);

		// Pass the new subtexture coordinates.
		auto coords = tex_info->get_subtex_info(subtex_idx).get_tile_params();
		this->uniforms->update("tile_params", coords);

		/* Model matrix */
		// TODO: Only update on resize
		auto model = Eigen::Affine3f::Identity();

		// scale and keep widthxheight ratio of texture
		// when the viewport size changes
		auto screen_size = this->camera->get_viewport_size();
		auto tex_size = tex_info->get_subtex_info(subtex_idx).get_size();

		// Use scalefactor from animation
		auto scale = anim_info->get_scalefactor();
		model.prescale(Eigen::Vector3f{
			scale * (static_cast<float>(tex_size[0]) / screen_size[0]),
			scale * (static_cast<float>(tex_size[1]) / screen_size[1]),
			1.0f});

		// TODO: Use actual position from coordinate system
		auto pos = this->position;
		// Positional offset using anchor point
		auto anchor = tex_info->get_subtex_info(subtex_idx).get_anchor_params();
		auto anchor_offset = Eigen::Vector3f(
			scale * (static_cast<float>(anchor[0]) / screen_size[0]),
			scale * (static_cast<float>(anchor[1]) / screen_size[1]),
			0.0f);
		pos = pos + anchor_offset;
		model.pretranslate(pos);
		auto model_m = model.matrix();

		this->uniforms->update("model", model_m);
	}
}

uint32_t WorldObject::get_id() {
	return this->ref_id;
}

const Eigen::Vector3f WorldObject::get_position() {
	return this->position;
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
