// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#include "demo_0.h"

#include "renderer/demo/util.h"
#include "renderer/gui/integration/public/gui_application_with_logger.h"
#include "renderer/opengl/window.h"
#include "renderer/render_pass.h"
#include "renderer/render_target.h"
#include "renderer/resources/mesh_data.h"
#include "renderer/resources/shader_source.h"
#include "renderer/shader_program.h"


namespace openage::renderer::tests {

void renderer_demo_0(const util::Path &path) {
	auto qtapp = std::make_shared<gui::GuiApplicationWithLogger>();

	window_settings settings;
	settings.width = 800;
	settings.height = 600;
	settings.debug = true;
	opengl::GlWindow window("openage renderer test", settings);
	auto renderer = window.make_renderer();

	auto shaderdir = path / "assets" / "test" / "shaders";

	/* Shader for copying the framebuffer in pass 2. */
	auto display_vshader_file = (shaderdir / "demo_0_display.vert.glsl").open();
	auto display_vshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		display_vshader_file.read());
	display_vshader_file.close();

	auto display_fshader_file = (shaderdir / "demo_0_display.frag.glsl").open();
	auto display_fshader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::fragment,
		display_fshader_file.read());
	display_fshader_file.close();

	auto display_shader = renderer->add_shader({display_vshader_src, display_fshader_src});

	auto quad = renderer->add_mesh_geometry(resources::MeshData::make_quad());
	Renderable display_stuff{
		display_shader->create_empty_input(),
		quad,
		false,
		false,
	};

	if (not check_uniform_completeness({display_stuff})) {
		log::log(WARN << "Uniforms not complete.");
	}

	auto pass = renderer->add_render_pass({display_stuff}, renderer->get_display_target());

	while (not window.should_close()) {
		renderer->render(pass);
		window.update();
		qtapp->process_events();

		renderer->check_error();
	}
	window.close();
}

} // namespace openage::renderer::tests
