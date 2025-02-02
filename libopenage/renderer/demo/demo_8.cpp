// Copyright 2023-2025 the openage authors. See copying.md for legal info.

#include "demo_8.h"

#include "renderer/demo/util.h"
#include "renderer/gui/gui.h"
#include "renderer/gui/integration/public/gui_application_with_logger.h"
#include "renderer/opengl/render_pass.h"
#include "renderer/opengl/renderer.h"
#include "renderer/opengl/window.h"
#include "renderer/render_pass.h"
#include "renderer/render_target.h"
#include "renderer/resources/mesh_data.h"
#include "renderer/resources/shader_source.h"
#include "renderer/shader_program.h"


namespace openage::renderer::tests {

void renderer_demo_8(const util::Path &path) {
	auto qtapp = std::make_shared<gui::GuiApplicationWithLogger>();

	// Create window and renderer
	window_settings settings;
	settings.width = 800;
	settings.height = 600;
	settings.debug = true;
	auto window = std::make_shared<opengl::GlWindow>("openage renderer stencil test", settings);
	auto renderer = window->make_renderer();

	auto shaderdir = path / "assets" / "test" / "shaders";

	// Create background shader (something that would be masked by GUI)
	auto bg_vshader_file = (shaderdir / "demo_8_gui_stencil.vert.glsl").open();
	auto bg_vshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		bg_vshader_file.read());
	bg_vshader_file.close();

	auto bg_fshader_file = (shaderdir / "demo_8_gui_stencil.frag.glsl").open();
	auto bg_fshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		bg_fshader_file.read());
	bg_fshader_file.close();

	// Load simple GUI quad shader from
	auto gui_vshader_file = (shaderdir / "demo_8_simple_gui.vert.glsl").open();
	auto gui_vshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		gui_vshader_file.read());
	gui_vshader_file.close();

	auto gui_fshader_file = (shaderdir / "demo_8_simple_gui.frag.glsl").open();
	auto gui_fshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		gui_fshader_file.read());
	gui_fshader_file.close();

	auto bg_shader = renderer->add_shader({bg_vshader_src, bg_fshader_src});
	auto gui_shader = renderer->add_shader({gui_vshader_src, gui_fshader_src});

	auto quad = renderer->add_mesh_geometry(resources::MeshData::make_quad());

	auto bg_uniforms = bg_shader->new_uniform_input();
	auto gui_uniforms = gui_shader->new_uniform_input();

	Renderable bg_obj{
		bg_uniforms,
		quad,
		false,
		true, // Enable depth test
	};

	Renderable gui_obj{
		gui_uniforms,
		quad,
		true,  // Enable alpha blending
		false, // No depth test needed
	};

	auto depth_stencil_texture = renderer->add_texture(
		resources::Texture2dInfo(
			settings.width, settings.height, resources::pixel_format::depth24_stencil8));
	auto stencil_fbo = renderer->create_texture_target({depth_stencil_texture});

	//auto bg_pass = renderer->add_render_pass({bg_obj}, renderer->get_display_target());
	auto bg_pass = renderer->add_render_pass({bg_obj}, renderer->get_display_target());
	auto gui_pass = renderer->add_render_pass({gui_obj}, renderer->get_display_target());
	auto gui_stencil_pass = renderer->add_render_pass({gui_obj}, stencil_fbo);

	// Configure stencil testing for background pass
	auto gl_bg_pass = std::dynamic_pointer_cast<opengl::GlRenderPass>(bg_pass);
	if (gl_bg_pass) {
		renderer::opengl::StencilConfig config;
		config.enabled = true;
		config.write = false;
		config.ref_value = 1;
		config.test_func = GL_NOTEQUAL;
		config.depth_pass = GL_KEEP;
		gl_bg_pass->set_stencil_config(config);
	}

	auto gl_gui_stencil_pass = std::dynamic_pointer_cast<opengl::GlRenderPass>(gui_stencil_pass);
	if (gl_gui_stencil_pass) {
		renderer::opengl::StencilConfig config;
		config.enabled = true;
		config.write = true;
		config.ref_value = 1;
		config.test_func = GL_ALWAYS;
		config.depth_pass = GL_REPLACE;
		gl_gui_stencil_pass->set_stencil_config(config);
	}

	log::log(INFO << "Stencil Test Demo Instructions:");
	log::log(INFO << "  1. GUI elements will create a stencil mask");
	log::log(INFO << "  2. Background will only render in non-GUI areas");

	float current_time = 0.0f;

	while (not window->should_close()) {
		current_time += 0.01f;
		bg_uniforms->update("time", current_time);

		// Render background and GUI
		renderer->render(gui_stencil_pass);

		renderer->render(bg_pass);

		renderer->render(gui_pass);

		window->update();
		qtapp->process_events();

		renderer->check_error();
	}
	window->close();
}

} // namespace openage::renderer::tests