// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "demo_0.h"

#include "renderer/gui/integration/public/gui_application_with_logger.h"
#include "renderer/opengl/window.h"
#include "renderer/renderer.h"
#include "renderer/resources/mesh_data.h"
#include "renderer/resources/shader_source.h"
#include "renderer/resources/texture_info.h"
#include "renderer/shader_program.h"


namespace openage::path::tests {

void path_demo_0(const util::Path &path) {
	auto qtapp = std::make_shared<renderer::gui::GuiApplicationWithLogger>();

	renderer::opengl::GlWindow window("openage pathfinding test", 1024, 768, true);
	auto renderer = window.make_renderer();

	auto shaderdir = path / "assets" / "test" / "shaders" / "pathfinding";

	/* Shader for rendering the cost field */
	auto cf_vshader_file = shaderdir / "demo_0_cost_field.vert.glsl";
	auto cf_vshader_src = renderer::resources::ShaderSource(
		renderer::resources::shader_lang_t::glsl,
		renderer::resources::shader_stage_t::vertex,
		cf_vshader_file);

	auto cf_fshader_file = shaderdir / "demo_0_cost_field.frag.glsl";
	auto cf_fshader_src = renderer::resources::ShaderSource(
		renderer::resources::shader_lang_t::glsl,
		renderer::resources::shader_stage_t::fragment,
		cf_fshader_file);

	/* Shader for rendering the integration field */
	auto if_vshader_file = shaderdir / "demo_0_integration_field.vert.glsl";
	auto if_vshader_src = renderer::resources::ShaderSource(
		renderer::resources::shader_lang_t::glsl,
		renderer::resources::shader_stage_t::vertex,
		if_vshader_file);

	auto if_fshader_file = shaderdir / "demo_0_integration_field.frag.glsl";
	auto if_fshader_src = renderer::resources::ShaderSource(
		renderer::resources::shader_lang_t::glsl,
		renderer::resources::shader_stage_t::fragment,
		if_fshader_file);

	/* Shader for rendering the flow field */
	auto ff_vshader_file = shaderdir / "demo_0_flow_field.vert.glsl";
	auto ff_vshader_src = renderer::resources::ShaderSource(
		renderer::resources::shader_lang_t::glsl,
		renderer::resources::shader_stage_t::vertex,
		ff_vshader_file);

	auto ff_fshader_file = shaderdir / "demo_0_flow_field.frag.glsl";
	auto ff_fshader_src = renderer::resources::ShaderSource(
		renderer::resources::shader_lang_t::glsl,
		renderer::resources::shader_stage_t::fragment,
		ff_fshader_file);

	/* Shader for monocolored objects. */
	auto obj_vshader_file = shaderdir / "demo_0_obj.vert.glsl";
	auto obj_vshader_src = renderer::resources::ShaderSource(
		renderer::resources::shader_lang_t::glsl,
		renderer::resources::shader_stage_t::vertex,
		obj_vshader_file);

	auto obj_fshader_file = shaderdir / "demo_0_obj.frag.glsl";
	auto obj_fshader_src = renderer::resources::ShaderSource(
		renderer::resources::shader_lang_t::glsl,
		renderer::resources::shader_stage_t::fragment,
		obj_fshader_file);

	/* Shader for rendering to the display target */
	auto display_vshader_file = shaderdir / "demo_0_display.vert.glsl";
	auto display_vshader_src = renderer::resources::ShaderSource(
		renderer::resources::shader_lang_t::glsl,
		renderer::resources::shader_stage_t::vertex,
		display_vshader_file);

	auto display_fshader_file = shaderdir / "demo_0_display.frag.glsl";
	auto display_fshader_src = renderer::resources::ShaderSource(
		renderer::resources::shader_lang_t::glsl,
		renderer::resources::shader_stage_t::fragment,
		display_fshader_file);

	// Create the shaders
	auto cf_shader = renderer->add_shader({cf_vshader_src, cf_fshader_src});
	auto if_shader = renderer->add_shader({if_vshader_src, if_fshader_src});
	auto ff_shader = renderer->add_shader({ff_vshader_src, ff_fshader_src});
	auto obj_shader = renderer->add_shader({obj_vshader_src, obj_fshader_src});
	auto display_shader = renderer->add_shader({display_vshader_src, display_fshader_src});

	/* Make a framebuffer for the field render pass to draw into. */
	auto size = window.get_size();
	auto color_texture = renderer->add_texture(
		renderer::resources::Texture2dInfo(size[0],
	                                       size[1],
	                                       renderer::resources::pixel_format::rgba8));
	auto depth_texture = renderer->add_texture(
		renderer::resources::Texture2dInfo(size[0],
	                                       size[1],
	                                       renderer::resources::pixel_format::depth24));
	auto fbo = renderer->create_texture_target({color_texture, depth_texture});

	auto obj_pass = renderer->add_render_pass({}, fbo);

	/* Make an object encompassing the entire screen for the display render pass */
	auto color_texture_unif = display_shader->new_uniform_input("color_texture", color_texture);
	auto quad = renderer->add_mesh_geometry(renderer::resources::MeshData::make_quad());
	renderer::Renderable display_obj{
		color_texture_unif,
		quad,
		false,
		false,
	};

	auto display_pass = renderer->add_render_pass({display_obj}, renderer->get_display_target());


	auto background_unifs = obj_shader->new_uniform_input(
		"color",
		Eigen::Vector4f{64.0 / 256, 128.0 / 256, 196.0 / 256, 1.0});
	auto background = renderer->add_mesh_geometry(renderer::resources::MeshData::make_quad());
	renderer::Renderable background_obj{
		background_unifs,
		background,
		false,
		false,
	};
	obj_pass->add_renderables(background_obj);


	while (not window.should_close()) {
		qtapp->process_events();

		renderer->render(obj_pass);
		renderer->render(display_pass);

		window.update();

		renderer->check_error();
	}
	window.close();
}

} // namespace openage::path::tests
