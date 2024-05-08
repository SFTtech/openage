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
#include "renderer/texture.h"
#include "renderer/uniform_input.h"
#include "renderer/window.h"
#include "util/path.h"

namespace openage::renderer::screen {

ScreenRenderStage::ScreenRenderStage(const std::shared_ptr<Window> & /* window */,
                                     const std::shared_ptr<renderer::Renderer> &renderer,
                                     const util::Path &shaderdir) :
	renderer{renderer},
	render_targets{},
	pass_outputs{} {
	renderer::opengl::GlContext::check_error();

	this->initialize_render_pass(shaderdir);

	log::log(INFO << "Created render stage 'Screen'");
}

std::shared_ptr<renderer::RenderPass> ScreenRenderStage::get_render_pass() {
	return this->render_pass;
}

void ScreenRenderStage::set_render_targets(const std::vector<std::shared_ptr<renderer::RenderTarget>> &targets) {
	this->render_targets = targets;
	this->update_render_pass();
}

void ScreenRenderStage::initialize_render_pass(const util::Path &shaderdir) {
	auto vert_shader_file = (shaderdir / "final.vert.glsl").open();
	auto vert_shader_src = renderer::resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		vert_shader_file.read());
	vert_shader_file.close();

	auto frag_shader_file = (shaderdir / "final.frag.glsl").open();
	auto frag_shader_src = renderer::resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		frag_shader_file.read());
	frag_shader_file.close();

	auto quad = renderer->add_mesh_geometry(renderer::resources::MeshData::make_quad());
	this->display_shader = renderer->add_shader({vert_shader_src, frag_shader_src});

	// nothing is drawn until render targets are added
	this->render_pass = renderer->add_render_pass({}, renderer->get_display_target());
}

void ScreenRenderStage::update_render_pass() {
	auto quad = renderer->add_mesh_geometry(renderer::resources::MeshData::make_quad());

	std::vector<renderer::Renderable> output_layers{};
	output_layers.reserve(this->render_targets.size());
	for (auto &target : this->render_targets) {
		auto textures = target->get_texture_targets();
		auto texture_unif = this->display_shader->create_empty_input();

		// TODO: Dirty hack that only selects color textures
		// use this->pass_outputs in the future to assign output
		// textures we want to render
		for (const auto &tex : textures) {
			auto format = tex->get_info().get_format();
			if (format == renderer::resources::pixel_format::rgba8) {
				texture_unif->update("tex", tex);
				break;
			}
		}
		renderer::Renderable display_obj{
			texture_unif,
			quad,
			true,
			false,
		};
		output_layers.push_back(display_obj);
	}
	this->render_pass->clear_renderables();
	this->render_pass->add_renderables(std::move(output_layers));
}

} // namespace openage::renderer::screen
