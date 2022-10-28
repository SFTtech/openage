// Copyright 2015-2022 the openage authors. See copying.md for legal info.

#include "renderer/gui/gui.h"

#include "renderer/gui/qml_info.h"
#include "renderer/opengl/context.h"
#include "renderer/renderer.h"
#include "renderer/resources/shader_source.h"
#include "renderer/resources/texture_info.h"
#include "renderer/window.h"
#include "util/path.h"

namespace openage::renderer::gui {

GUI::GUI(std::shared_ptr<qtgui::GuiApplication> app,
         std::shared_ptr<Window> window,
         const util::Path &source,
         const util::Path &rootdir,
         const util::Path &assetdir,
         const std::shared_ptr<Renderer> &renderer,
         QMLInfo *info) :
	application{app},
	render_updater{}, gui_renderer{window}, renderer{renderer}
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
	// everything alright before we create the gui stuff?
	renderer::opengl::GlContext::check_error();

	auto size = window->get_size();
	// create the appropriate texture
	this->resize(size[0], size[1]);

	util::Path shaderdir = assetdir["shaders"];
	this->initialize_render_pass(size[0], size[1], shaderdir);

	/*
	window->add_key_callback([&](const QKeyEvent &event) {
		this->input.process(&ev);
	});
	window->add_mouse_button_callback([&](const QMouseEvent &event) {
		this->input.process(&ev);
	});
	window->add_mouse_wheel_callback([&](const QWheelEvent &event) {
		this->input.process(&ev);
	});
	*/
	window->add_resize_callback([this](size_t width, size_t height) {
		this->resize(width, height);
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
                                 const util::Path &shaderdir) {
	auto id_vert_file = (shaderdir / "identity.vert.glsl").open();
	auto id_shader_src = renderer::resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		id_vert_file.read());
	id_vert_file.close();

	auto text_frag_file = (shaderdir / "maptexture.frag.glsl").open();
	auto maptex_frag_shader_src = renderer::resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		text_frag_file.read());
	text_frag_file.close();

	auto quad = this->renderer->add_mesh_geometry(resources::MeshData::make_quad());
	auto maptex_shader = this->renderer->add_shader({id_shader_src, maptex_frag_shader_src});

	// GUI draw surface. gets drawn on top of the gameworld in the presenter.
	auto fbo = this->renderer->create_texture_target({this->texture});

	auto color_texture_unif = maptex_shader->new_uniform_input("texture", this->texture);
	Renderable display_obj{
		color_texture_unif,
		quad,
		true,
		true,
	};

	// TODO: Render into FBO instead of screen
	this->render_pass = renderer->add_render_pass({display_obj}, renderer->get_display_target());
}


void GUI::resize(size_t width, size_t height) {
	// TODO: this texture has to be bigger because of highdpi scaling
	//       width and height are not the real window pixel surface.
	// Hint: Use effectiveDevicePixelRatio() method
	this->texture = this->renderer->add_texture(
		resources::Texture2dInfo(width, height, resources::pixel_format::rgba8));

	//fbo = this->renderer->create_texture_target( { this->texture } );
	//this->render_pass->set_target(fbo)

	// update new texture to gui renderer
	this->gui_renderer.resize(width, height);
	this->gui_renderer.set_texture(this->texture);
}


void GUI::render() {
	this->render_updater.process_callbacks();

	this->gui_renderer.render();
}

} // namespace openage::renderer::gui
