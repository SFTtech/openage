// Copyright 2022-2024 the openage authors. See copying.md for legal info.

#include "render_stage.h"

#include "renderer/opengl/context.h"
#include "renderer/render_pass.h"
#include "renderer/render_target.h"
#include "renderer/renderer.h"
#include "renderer/resources/mesh_data.h"
#include "renderer/resources/shader_source.h"
#include "renderer/resources/texture_info.h"
#include "renderer/shader_program.h"
#include "renderer/uniform_input.h"
#include "renderer/window.h"
#include "util/path.h"

namespace openage::renderer::skybox {

SkyboxRenderStage::SkyboxRenderStage(const std::shared_ptr<Window> &window,
                                     const std::shared_ptr<renderer::Renderer> &renderer,
                                     const util::Path &shaderdir) :
	renderer{renderer},
	bg_color{0.0, 0.0, 0.0, 1.0} // black
{
	renderer::opengl::GlContext::check_error();

	auto size = window->get_size();
	this->initialize_render_pass(size[0], size[1], shaderdir);

	window->add_resize_callback([this](size_t width, size_t height, double /*scale*/) {
		this->resize(width, height);
	});

	log::log(INFO << "Created render stage 'Skybox'");
}

std::shared_ptr<renderer::RenderPass> SkyboxRenderStage::get_render_pass() {
	return this->render_pass;
}

void SkyboxRenderStage::set_color(const Eigen::Vector4i col) {
	this->bg_color = Eigen::Vector4f(
		col[0] / 255,
		col[1] / 255,
		col[2] / 255,
		col[3] / 255);
	this->color_unif->update("in_col", this->bg_color);
}

void SkyboxRenderStage::set_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	this->bg_color = Eigen::Vector4f(
		r / 255,
		g / 255,
		b / 255,
		a / 255);
	this->color_unif->update("in_col", this->bg_color);
}

void SkyboxRenderStage::set_color(const Eigen::Vector4f col) {
	this->bg_color = col;
	this->color_unif->update("in_col", this->bg_color);
}

void SkyboxRenderStage::set_color(float r, float g, float b, float a) {
	this->bg_color = Eigen::Vector4f(r, g, b, a);
	this->color_unif->update("in_col", this->bg_color);
}

void SkyboxRenderStage::resize(size_t width, size_t height) {
	this->output_texture = renderer->add_texture(resources::Texture2dInfo(width, height, resources::pixel_format::rgba8));

	auto fbo = this->renderer->create_texture_target({this->output_texture});
	this->render_pass->set_target(fbo);
}

void SkyboxRenderStage::initialize_render_pass(size_t width,
                                               size_t height,
                                               const util::Path &shaderdir) {
	auto vert_shader_file = (shaderdir / "skybox.vert.glsl").open();
	auto vert_shader_src = renderer::resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		vert_shader_file.read());
	vert_shader_file.close();

	auto frag_shader_file = (shaderdir / "skybox.frag.glsl").open();
	auto frag_shader_src = renderer::resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		frag_shader_file.read());
	frag_shader_file.close();

	this->output_texture = renderer->add_texture(resources::Texture2dInfo(width, height, resources::pixel_format::rgba8));

	auto geometry = this->renderer->add_mesh_geometry(resources::MeshData::make_quad());
	auto shader = this->renderer->add_shader({vert_shader_src, frag_shader_src});

	this->color_unif = shader->new_uniform_input("in_col", this->bg_color);
	Renderable display_obj{
		color_unif,
		geometry,
		false,
		false,
	};

	auto fbo = this->renderer->create_texture_target({this->output_texture});
	this->render_pass = this->renderer->add_render_pass({display_obj}, fbo);
}

} // namespace openage::renderer::skybox
