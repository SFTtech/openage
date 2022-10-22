// Copyright 2015-2022 the openage authors. See copying.md for legal info.

#include "renderer/gui/gui.h"

#include "renderer/gui/qml_info.h"
#include "renderer/renderer.h"
#include "renderer/resources/shader_source.h"
#include "renderer/resources/texture_info.h"
#include "renderer/window.h"
#include "util/path.h"

#include "renderer/opengl/context_qt.h"

namespace openage {
namespace renderer {
namespace gui {

GUI::GUI(std::shared_ptr<qtgui::GuiApplication> app,
         std::shared_ptr<Window> window,
         const util::Path &source,
         const util::Path &rootdir,
         const util::Path &assetdir,
         std::shared_ptr<Renderer> renderer,
         QMLInfo *info) :
	application{app},
	render_updater{},
	gui_renderer{window}
//game_logic_updater{},
//image_provider_by_filename{
//	&render_updater,
//	openage::gui::GuiGameSpecImageProvider::Type::ByFilename},
//engine{
//	&gui_renderer,
//	{&image_provider_by_filename},
//	info},
//subtree{
//	&gui_renderer,
//	&game_logic_updater,
//	&engine,
//	source.resolve_native_path(),
//	rootdir.resolve_native_path()},
//input{&gui_renderer, &game_logic_updater}
{
	renderer::opengl::QGlContext::check_error(); // failure

	util::Path shaderdir = assetdir["shaders"];

	auto size = window->get_size();
	initialize_render_pass(size[0], size[1], renderer, shaderdir);

	/*
	window->add_key_callback([&](SDL_KeyboardEvent const &event) {
		auto ev = *reinterpret_cast<SDL_Event const *>(&event);
		// this->input.process(&ev);
	});
	window->add_mouse_button_callback([&](SDL_MouseButtonEvent const &event) {
		auto ev = *reinterpret_cast<SDL_Event const *>(&event);
		// this->input.process(&ev);
	});
	window->add_mouse_wheel_callback([&](SDL_MouseWheelEvent const &event) {
		auto ev = *reinterpret_cast<SDL_Event const *>(&event);
		// this->input.process(&ev);
	});
	*/
	window->add_resize_callback([&](size_t width, size_t height) {
		this->gui_renderer.resize(width, height);
	});
}

std::shared_ptr<renderer::RenderPass> GUI::get_render_pass() {
	return this->render_pass;
}

void GUI::process_events() {
	// this->game_logic_updater.process_callbacks();
	this->application->process_events();
}

void GUI::initialize_render_pass(size_t width,
                                 size_t height,
                                 std::shared_ptr<Renderer> renderer,
                                 const util::Path &shaderdir) {
	const std::string shader_header_code = "#version 120\n";

	auto id_vert_file = (shaderdir / "identity.vert.glsl").open();
	auto id_shader_src = renderer::resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		shader_header_code + id_vert_file.read());
	id_vert_file.close();

	auto text_frag_file = (shaderdir / "maptexture.frag.glsl").open();
	auto maptex_frag_shader_src = renderer::resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		shader_header_code + text_frag_file.read());
	text_frag_file.close();

	auto quad = renderer->add_mesh_geometry(resources::MeshData::make_quad());
	auto maptex_shader = renderer->add_shader({id_shader_src, maptex_frag_shader_src});
	this->textured_screen_quad_shader = maptex_shader;

	auto gui_texture = renderer->add_texture(
		resources::Texture2dInfo(width, height, resources::pixel_format::rgba8));
	auto fbo = renderer->create_texture_target({gui_texture});

	auto color_texture_unif = maptex_shader->new_uniform_input("texture", gui_texture);
	Renderable display_obj{
		color_texture_unif,
		quad,
		true,
		true,
	};

	this->render_pass = renderer->add_render_pass({/* display_obj */}, renderer->get_display_target());
}

bool GUI::drawhud() {
	this->render_updater.process_callbacks();

	auto tex = this->gui_renderer.render();

	// glEnable(GL_BLEND);
	// glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	// this->textured_screen_quad_shader->use();

	// glActiveTexture(GL_TEXTURE0);
	// glBindTexture(GL_TEXTURE_2D, tex);

	// glEnableVertexAttribArray(this->textured_screen_quad_shader->pos_id);

	// glBindBuffer(GL_ARRAY_BUFFER, this->screen_quad_vbo);
	// glVertexAttribPointer(
	// 	this->textured_screen_quad_shader->pos_id,
	// 	2,
	// 	GL_FLOAT,
	// 	GL_FALSE,
	// 	2 * sizeof(float),
	// 	0);

	// glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	// glDisableVertexAttribArray(this->textured_screen_quad_shader->pos_id);
	// glBindBuffer(GL_ARRAY_BUFFER, 0);

	// glBindTexture(GL_TEXTURE_2D, 0);

	// this->textured_screen_quad_shader->stopusing();

	return true;
}

} // namespace gui
} // namespace renderer
} // namespace openage
