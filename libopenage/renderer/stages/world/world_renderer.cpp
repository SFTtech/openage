// Copyright 2022-2022 the openage authors. See copying.md for legal info.

#include "world_renderer.h"

#include "renderer/opengl/context.h"
#include "renderer/resources/shader_source.h"
#include "renderer/resources/texture_info.h"
#include "renderer/shader_program.h"
#include "renderer/stages/world/world_object.h"
#include "renderer/window.h"

namespace openage::renderer::world {

WorldRenderer::WorldRenderer(const std::shared_ptr<Window> &window,
                             const std::shared_ptr<renderer::Renderer> &renderer,
                             const util::Path &shaderdir) :
	renderer{renderer},
	render_objects{} {
	renderer::opengl::GlContext::check_error();

	auto size = window->get_size();
	this->initialize_render_pass(size[0], size[1], shaderdir);

	window->add_resize_callback([this](size_t width, size_t height) {
		this->resize(width, height);
	});
}

std::shared_ptr<renderer::RenderPass> WorldRenderer::get_render_pass() {
	return this->render_pass;
}

void WorldRenderer::add_render_entity(const std::shared_ptr<WorldRenderEntity> &entity) {
	auto world_object = std::make_shared<WorldObject>();
	world_object->set_render_entity(entity);
	this->render_objects.push_back(world_object);

	this->update();
}

void WorldRenderer::update() {
	for (auto obj : this->render_objects) {
		obj->update();

		if (obj->is_changed()) {
			if (obj->requires_renderable()) {
				// TODO: Update existing renderable instead of recreating it
				auto geometry = this->renderer->add_mesh_geometry(obj->get_mesh());
				auto transform_unifs = this->display_shader->create_empty_input();

				Renderable display_obj{
					transform_unifs, // TODO
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

	auto fbo = this->renderer->create_texture_target({this->output_texture});
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

	this->display_shader = this->renderer->add_shader({vert_shader_src, frag_shader_src});

	auto fbo = this->renderer->create_texture_target({this->output_texture});
	this->render_pass = this->renderer->add_render_pass({/* display_obj */}, fbo);
}

} // namespace openage::renderer::world