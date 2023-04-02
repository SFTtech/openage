// Copyright 2022-2023 the openage authors. See copying.md for legal info.

#include "world_renderer.h"

#include "event/clock.h"
#include "renderer/opengl/context.h"
#include "renderer/resources/assets/asset_manager.h"
#include "renderer/resources/shader_source.h"
#include "renderer/resources/texture_info.h"
#include "renderer/shader_program.h"
#include "renderer/stages/world/world_object.h"
#include "renderer/texture.h"
#include "renderer/window.h"

namespace openage::renderer::world {

WorldRenderer::WorldRenderer(const std::shared_ptr<Window> &window,
                             const std::shared_ptr<renderer::Renderer> &renderer,
                             const util::Path &shaderdir,
                             const std::shared_ptr<renderer::resources::AssetManager> &asset_manager,
                             const std::shared_ptr<event::Clock> clock) :
	renderer{renderer},
	asset_manager{asset_manager},
	render_objects{},
	clock{clock} {
	renderer::opengl::GlContext::check_error();

	auto size = window->get_size();
	this->initialize_render_pass(size[0], size[1], shaderdir);

	window->add_resize_callback([this](size_t width, size_t height) {
		this->resize(width, height);
	});

	// TODO: Mouse button callbacks for analyzing ID texture
}

std::shared_ptr<renderer::RenderPass> WorldRenderer::get_render_pass() {
	return this->render_pass;
}

void WorldRenderer::add_render_entity(const std::shared_ptr<WorldRenderEntity> entity) {
	std::unique_lock lock{this->mutex};

	auto world_object = std::make_shared<WorldObject>(this->asset_manager);
	world_object->set_render_entity(entity);
	this->render_objects.push_back(world_object);

	this->update();
}

void WorldRenderer::update() {
	auto current_time = this->clock->get_real_time();
	for (auto obj : this->render_objects) {
		obj->update(current_time);

		if (obj->is_changed()) {
			if (obj->requires_renderable()) {
				// TODO: Update existing renderable instead of recreating it
				auto geometry = this->renderer->add_mesh_geometry(obj->get_mesh());

				// TODO: Use correct mvp matrices
				auto model = Eigen::Affine3f::Identity();
				auto screen_size = this->output_texture->get_info().get_size();
				auto tex_size = obj->get_texture()->get_info().get_size();
				// scale and keep widthxheight ratio of texture
				auto scale = 1.0f;
				if (tex_size.first > 100) {
					scale = 0.15f;
				}
				model.prescale(Eigen::Vector3f{
					scale * ((float)tex_size.first / screen_size.first),
					scale * ((float)tex_size.second / screen_size.second),
					1.0f});
				auto pos = obj->get_position();
				pos = pos + Eigen::Vector3f(-5.0f, -5.0f, 0.0f);
				pos = pos * 0.1f;
				model.pretranslate(pos);
				auto model_m = model.matrix();
				Eigen::Matrix4f view_m = Eigen::Matrix4f::Identity();
				Eigen::Matrix4f proj_m = Eigen::Matrix4f::Identity();

				// TODO: Use subtex coordinates instead of whole texture
				auto offsets = Eigen::Vector4f{0.0f, 1.0f, 0.0f, 1.0f};

				auto transform_unifs = this->display_shader->new_uniform_input(
					"model",
					model_m,
					"view",
					view_m,
					"proj",
					proj_m,
					"offset_tile",
					offsets,
					"tex",
					obj->get_texture(),
					"u_id",
					obj->get_id());

				Renderable display_obj{
					transform_unifs,
					geometry,
					true,
					true,
				};

				this->render_pass->add_renderables(display_obj);

				obj->set_uniforms(transform_unifs);
				obj->clear_requires_renderable();
			}
		}
	}
}

void WorldRenderer::resize(size_t width, size_t height) {
	this->output_texture = renderer->add_texture(resources::Texture2dInfo(width, height, resources::pixel_format::rgba8));
	this->id_texture = renderer->add_texture(resources::Texture2dInfo(width, height, resources::pixel_format::r32ui));

	auto fbo = this->renderer->create_texture_target({this->output_texture, this->id_texture});
	this->render_pass->set_target(fbo);
}

void WorldRenderer::initialize_render_pass(size_t width,
                                           size_t height,
                                           const util::Path &shaderdir) {
	auto vert_shader_file = (shaderdir / "world.vert.glsl").open();
	auto vert_shader_src = renderer::resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		vert_shader_file.read());
	vert_shader_file.close();

	auto frag_shader_file = (shaderdir / "world.frag.glsl").open();
	auto frag_shader_src = renderer::resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		frag_shader_file.read());
	frag_shader_file.close();

	this->output_texture = renderer->add_texture(resources::Texture2dInfo(width, height, resources::pixel_format::rgba8));
	this->id_texture = renderer->add_texture(resources::Texture2dInfo(width, height, resources::pixel_format::r32ui));

	this->display_shader = this->renderer->add_shader({vert_shader_src, frag_shader_src});

	auto fbo = this->renderer->create_texture_target({this->output_texture, this->id_texture});
	this->render_pass = this->renderer->add_render_pass({}, fbo);
}

} // namespace openage::renderer::world
