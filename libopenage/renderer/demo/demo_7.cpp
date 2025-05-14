// Copyright 2025-2025 the openage authors. See copying.md for legal info.

#include "demo_7.h"

#include "util/path.h"

#include "renderer/demo/util.h"
#include "renderer/gui/integration/public/gui_application_with_logger.h"
#include "renderer/opengl/window.h"
#include "renderer/render_pass.h"
#include "renderer/render_target.h"
#include "renderer/resources/mesh_data.h"
#include "renderer/resources/shader_source.h"
#include "renderer/resources/shader_template.h"
#include "renderer/shader_program.h"


namespace openage::renderer::tests {

void renderer_demo_7(const util::Path &path) {
	// Basic setup
	auto qtapp = std::make_shared<gui::GuiApplicationWithLogger>();
	window_settings settings;
	settings.width = 800;
	settings.height = 600;
	settings.debug = true;

	opengl::GlWindow window("Shader Commands Demo", settings);
	auto renderer = window.make_renderer();

	auto shaderdir = path / "assets" / "test" / "shaders";

	// Initialize shader template
	resources::ShaderTemplate frag_template(shaderdir / "demo_7_shader_command.frag.glsl");

	// Load snippets from a snippet directory
	frag_template.load_snippets(shaderdir / "demo_7_snippets");

	auto vert_shader_file = (shaderdir / "demo_7_shader_command.vert.glsl").open();
	auto vert_shader_src = resources::ShaderSource(
		resources::shader_lang_t::glsl,
		resources::shader_stage_t::vertex,
		vert_shader_file.read());
	vert_shader_file.close();

	auto frag_shader_src = frag_template.generate_source();

	auto shader = renderer->add_shader({vert_shader_src, frag_shader_src});

	// Create a simple quad for rendering
	auto quad = renderer->add_mesh_geometry(resources::MeshData::make_quad());

	auto uniforms = shader->new_uniform_input("time", 0.0f);

	Renderable display_obj{
		uniforms,
		quad,
		false,
		false,
	};

	if (not check_uniform_completeness({display_obj})) {
		log::log(WARN << "Uniforms not complete.");
	}

	auto pass = renderer->add_render_pass({display_obj}, renderer->get_display_target());

	// Main loop
	float time = 0.0f;
	while (not window.should_close()) {
		time += 0.016f;
		uniforms->update("time", time);

		renderer->render(pass);
		window.update();
		qtapp->process_events();

		renderer->check_error();
	}
	window.close();
}

} // namespace openage::renderer::tests
