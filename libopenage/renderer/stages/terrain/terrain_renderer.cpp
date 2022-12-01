// Copyright 2022-2022 the openage authors. See copying.md for legal info.

#include "terrain_renderer.h"


#include "renderer/opengl/context.h"
#include "renderer/renderer.h"
#include "renderer/resources/shader_source.h"
#include "renderer/resources/texture_info.h"
#include "renderer/shader_program.h"
#include "renderer/stages/terrain/terrain_mesh.h"
#include "renderer/stages/terrain/terrain_model.h"
#include "renderer/stages/terrain/terrain_render_entity.h"
#include "renderer/window.h"

namespace openage::renderer::terrain {

TerrainRenderer::TerrainRenderer(const std::shared_ptr<Window> &window,
                                 const std::shared_ptr<renderer::Renderer> &renderer,
                                 const util::Path &shaderdir) :
	renderer{renderer},
	render_entity{},
	model{std::make_shared<TerrainRenderModel>()},
	mesh{std::make_shared<TerrainRenderMesh>(renderer, this->render_entity)} {
	renderer::opengl::GlContext::check_error();

	auto size = window->get_size();
	this->initialize_render_pass(size[0], size[1], shaderdir);

	window->add_resize_callback([this](size_t width, size_t height) {
		this->resize(width, height);
	});
}

std::shared_ptr<renderer::RenderPass> TerrainRenderer::get_render_pass() {
	return this->render_pass;
}

void TerrainRenderer::set_render_entity(const std::shared_ptr<TerrainRenderEntity> &entity) {
	this->render_entity = entity;
	this->mesh->set_render_entity(this->render_entity);
	this->update();
}

void TerrainRenderer::update() {
	this->mesh->update();

	if (this->mesh->is_changed()) {
		if (this->mesh->requires_renderable()) [[unlikely]] /*probably doesn't happen that often?*/ {
			// TODO: Update uniforms and geometry individually, depending on what changed
			// TODO: Update existing renderable instead of recreating it
			auto geometry = this->renderer->add_mesh_geometry(this->mesh->get_mesh());
			auto transform_unifs = this->display_shader->new_uniform_input(
				"model", // local space -> world space
				this->model->get_model_matrix(),
				"view", // camera
				this->model->get_view_matrix(),
				"proj", // orthographic view
				this->model->get_proj_matrix(),
				"tex", // terrain texture
				this->mesh->get_texture());

			Renderable display_obj{
				transform_unifs,
				geometry,
				true,
				true, // it's a 3D object, so we need depth testing
			};

			this->render_pass->clear_renderables();
			this->render_pass->add_renderables(display_obj);

			this->mesh->set_uniforms(transform_unifs);
			this->mesh->clear_requires_renderable();
		}
	}
}

void TerrainRenderer::resize(size_t width, size_t height) {
	this->output_texture = renderer->add_texture(resources::Texture2dInfo(width, height, resources::pixel_format::rgba8));

	auto fbo = this->renderer->create_texture_target({this->output_texture});
	this->render_pass->set_target(fbo);
}

void TerrainRenderer::initialize_render_pass(size_t width,
                                             size_t height,
                                             const util::Path &shaderdir) {
	auto vert_shader_file = (shaderdir / "terrain.vert.glsl").open();
	auto vert_shader_src = renderer::resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		vert_shader_file.read());
	vert_shader_file.close();

	auto frag_shader_file = (shaderdir / "terrain.frag.glsl").open();
	auto frag_shader_src = renderer::resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		frag_shader_file.read());
	frag_shader_file.close();

	this->output_texture = renderer->add_texture(resources::Texture2dInfo(width, height, resources::pixel_format::rgba8));

	this->display_shader = this->renderer->add_shader({vert_shader_src, frag_shader_src});

	auto fbo = this->renderer->create_texture_target({this->output_texture});
	this->render_pass = this->renderer->add_render_pass({}, fbo);
}

} // namespace openage::renderer::terrain