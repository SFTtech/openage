// Copyright 2022-2025 the openage authors. See copying.md for legal info.

#include "render_stage.h"

#include "renderer/camera/camera.h"
#include "renderer/camera/frustum_3d.h"
#include "renderer/opengl/context.h"
#include "renderer/render_pass.h"
#include "renderer/render_target.h"
#include "renderer/resources/assets/asset_manager.h"
#include "renderer/resources/shader_source.h"
#include "renderer/resources/texture_info.h"
#include "renderer/shader_program.h"
#include "renderer/stages/world/object.h"
#include "renderer/texture.h"
#include "renderer/window.h"
#include "time/clock.h"


namespace openage::renderer::world {

bool WorldRenderStage::ENABLE_FRUSTUM_CULLING = false;

WorldRenderStage::WorldRenderStage(const std::shared_ptr<Window> &window,
                                   const std::shared_ptr<renderer::Renderer> &renderer,
                                   const std::shared_ptr<renderer::camera::Camera> &camera,
                                   const util::Path &shaderdir,
                                   const std::shared_ptr<renderer::resources::AssetManager> &asset_manager,
                                   const std::shared_ptr<time::Clock> clock) :
	renderer{renderer},
	camera{camera},
	asset_manager{asset_manager},
	render_objects{},
	clock{clock},
	default_geometry{this->renderer->add_mesh_geometry(WorldObject::get_mesh())} {
	renderer::opengl::GlContext::check_error();

	auto size = window->get_size();
	this->initialize_render_pass(size[0], size[1], shaderdir);
	this->init_uniform_ids();

	window->add_resize_callback([this](size_t width, size_t height, double /*scale*/) {
		this->resize(width, height);
	});

	log::log(INFO << "Created render stage 'World'");
}

std::shared_ptr<renderer::RenderPass> WorldRenderStage::get_render_pass() {
	return this->render_pass;
}

void WorldRenderStage::add_render_entity(const std::shared_ptr<RenderEntity> entity) {
	std::unique_lock lock{this->mutex};

	auto world_object = std::make_shared<WorldObject>(this->asset_manager);
	world_object->set_render_entity(entity);
	this->render_objects.push_back(world_object);
}

void WorldRenderStage::update() {
	std::unique_lock lock{this->mutex};
	auto current_time = this->clock->get_real_time();
	auto &camera_frustum = this->camera->get_frustum_2d();
	for (auto &obj : this->render_objects) {
		obj->fetch_updates(current_time);

		if (WorldRenderStage::ENABLE_FRUSTUM_CULLING
		    and not obj->is_visible(camera_frustum, current_time)) {
			continue;
		}

		if (obj->is_changed()) {
			if (obj->requires_renderable()) {
				auto layer_positions = obj->get_layer_positions(current_time);
				static const Eigen::Matrix4f model_matrix = obj->get_model_matrix();

				std::vector<std::shared_ptr<renderer::UniformInput>> transform_unifs;
				for (auto layer_pos : layer_positions) {
					// Set uniforms that don't change or are not changed often
					auto layer_unifs = this->display_shader->new_uniform_input(
						"model",
						model_matrix,
						"flip_x",
						false,
						"flip_y",
						false,
						"u_id",
						obj->get_id());

					Renderable display_obj{
						layer_unifs,
						this->default_geometry,
						true,
						true,
					};
					this->render_pass->add_renderables(std::move(display_obj), layer_pos);
					transform_unifs.push_back(layer_unifs);
				}

				obj->clear_requires_renderable();

				// update remaining uniforms for the object
				obj->set_uniforms(std::move(transform_unifs));
			}
		}
		obj->update_uniforms(current_time);
	}
}

void WorldRenderStage::resize(size_t width, size_t height) {
	this->output_texture = renderer->add_texture(resources::Texture2dInfo(width, height, resources::pixel_format::rgba8));
	this->depth_texture = renderer->add_texture(resources::Texture2dInfo(width, height, resources::pixel_format::depth24));
	this->id_texture = renderer->add_texture(resources::Texture2dInfo(width, height, resources::pixel_format::r32ui));

	auto fbo = this->renderer->create_texture_target({this->output_texture, this->depth_texture, this->id_texture});
	this->render_pass->set_target(fbo);
}

void WorldRenderStage::initialize_render_pass(size_t width,
                                              size_t height,
                                              const util::Path &shaderdir) {
	auto vert_shader_file = (shaderdir / "world2d.vert.glsl").open();
	auto vert_shader_src = renderer::resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		vert_shader_file.read());
	vert_shader_file.close();

	auto frag_shader_file = (shaderdir / "world2d.frag.glsl").open();
	auto frag_shader_src = renderer::resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		frag_shader_file.read());
	frag_shader_file.close();

	this->output_texture = renderer->add_texture(resources::Texture2dInfo(width, height, resources::pixel_format::rgba8));
	this->depth_texture = renderer->add_texture(resources::Texture2dInfo(width, height, resources::pixel_format::depth24));
	this->id_texture = renderer->add_texture(resources::Texture2dInfo(width, height, resources::pixel_format::r32ui));

	this->display_shader = this->renderer->add_shader({vert_shader_src, frag_shader_src});
	this->display_shader->bind_uniform_buffer("camera", this->camera->get_uniform_buffer());

	auto fbo = this->renderer->create_texture_target({this->output_texture, this->depth_texture, this->id_texture});
	this->render_pass = this->renderer->add_render_pass({}, fbo);
}

void WorldRenderStage::init_uniform_ids() {
	WorldObject::obj_world_position = this->display_shader->get_uniform_id("obj_world_position");
	WorldObject::flip_x = this->display_shader->get_uniform_id("flip_x");
	WorldObject::flip_y = this->display_shader->get_uniform_id("flip_y");
	WorldObject::tex = this->display_shader->get_uniform_id("tex");
	WorldObject::tile_params = this->display_shader->get_uniform_id("tile_params");
	WorldObject::scale = this->display_shader->get_uniform_id("scale");
	WorldObject::subtex_size = this->display_shader->get_uniform_id("subtex_size");
	WorldObject::anchor_offset = this->display_shader->get_uniform_id("anchor_offset");
}

} // namespace openage::renderer::world
