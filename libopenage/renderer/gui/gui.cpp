// Copyright 2015-2024 the openage authors. See copying.md for legal info.

#include "gui.h"

#include "renderer/gui/guisys/public/gui_engine.h"
#include "renderer/gui/guisys/public/gui_input.h"
#include "renderer/gui/guisys/public/gui_renderer.h"
#include "renderer/gui/integration/public/gui_application_with_logger.h"
#include "renderer/opengl/context.h"
#include "renderer/render_pass.h"
#include "renderer/render_target.h"
#include "renderer/renderer.h"
#include "renderer/resources/shader_source.h"
#include "renderer/resources/texture_info.h"
#include "renderer/shader_program.h"
#include "renderer/window.h"
#include "util/path.h"


namespace openage::renderer::gui {

GUI::GUI(std::shared_ptr<qtgui::GuiApplication> app,
         std::shared_ptr<Window> window,
         const util::Path &source,
         const util::Path &rootdir,
         const util::Path &assetdir,
         const std::shared_ptr<Renderer> &renderer) :
	application{app},
	gui_renderer{std::make_shared<qtgui::GuiRenderer>(window)},
	gui_input{std::make_shared<qtgui::GuiInput>(gui_renderer)},
	engine{std::make_shared<qtgui::GuiQmlEngine>(gui_renderer)},
	subtree{
		gui_renderer,
		engine,
		source.resolve_native_path(),
		rootdir.resolve_native_path()},
	// input{&gui_renderer, &game_logic_updater}
    // image_provider_by_filename{
    //	&render_updater,
    //	openage::gui::GuiGameSpecImageProvider::Type::ByFilename},
	renderer{renderer} {
	// everything alright before we create the gui stuff?
	renderer::opengl::GlContext::check_error();

	auto size = window->get_size();
	// create the appropriate texture
	this->resize(size[0], size[1]);

	util::Path shaderdir = assetdir["shaders"];
	this->initialize_render_pass(size[0], size[1], shaderdir);

	window->add_resize_callback([this](size_t width, size_t height, double /*scale*/) {
		this->resize(width, height);
	});
}

std::shared_ptr<qtgui::GuiInput> GUI::get_input_handler() const {
	return this->gui_input;
}

std::shared_ptr<renderer::RenderPass> GUI::get_render_pass() const {
	return this->render_pass;
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
	auto output_texture = this->renderer->add_texture(
		resources::Texture2dInfo(width, height, resources::pixel_format::rgba8));
	auto fbo = this->renderer->create_texture_target({output_texture});

	this->texture_unif = maptex_shader->new_uniform_input("texture", this->texture);
	Renderable display_obj{
		this->texture_unif,
		quad,
		true,
		true,
	};

	// TODO: Rendering into the FBO is a bit redundant right now because we
	// just copy the GUI texture into the output texture
	this->render_pass = renderer->add_render_pass({display_obj}, fbo);
}


void GUI::resize(size_t width, size_t height) {
	// TODO: this texture has to be bigger because of highdpi scaling
	//       width and height are not the real window pixel surface.
	// Hint: Use effectiveDevicePixelRatio() method
	this->texture = this->renderer->add_texture(
		resources::Texture2dInfo(width, height, resources::pixel_format::rgba8));

	// update new texture to gui renderer
	this->gui_renderer->resize(width, height);
	this->gui_renderer->set_texture(this->texture);

	// update the fbo size of the render pass
	if (this->render_pass) {
		auto output_texture = this->renderer->add_texture(
			resources::Texture2dInfo(width, height, resources::pixel_format::rgba8));
		auto fbo = renderer->create_texture_target({output_texture});

		// pass the new texture to shader uniform
		this->texture_unif->update("texture", this->texture);

		this->render_pass->set_target(fbo);
	}
}


void GUI::render() {
	// this->render_updater.process_callbacks();

	this->gui_renderer->render();
}

} // namespace openage::renderer::gui
