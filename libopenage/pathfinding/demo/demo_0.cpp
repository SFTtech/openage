// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "demo_0.h"

#include "pathfinding/cost_field.h"
#include "pathfinding/flow_field.h"
#include "pathfinding/integration_field.h"
#include "renderer/camera/camera.h"
#include "renderer/gui/integration/public/gui_application_with_logger.h"
#include "renderer/opengl/window.h"
#include "renderer/renderer.h"
#include "renderer/resources/mesh_data.h"
#include "renderer/resources/shader_source.h"
#include "renderer/resources/texture_info.h"
#include "renderer/shader_program.h"
#include "util/vector.h"


namespace openage::path::tests {

/**
 * Create a mesh for the cost field.
 *
 * @param field Cost field to visualize.
 * @param resolution Determines the number of subdivisions per grid cell. The number of
 *                   quads per cell is resolution^2. (default = 2)
 *
 * @return Mesh data for the cost field.
 */
renderer::resources::MeshData get_cost_field_mesh(const std::shared_ptr<CostField> &field,
                                                  size_t resolution = 2) {
	// increase by 1 in every dimension because to get the vertex length
	// of each dimension
	util::Vector2s size{
		field->get_size() * resolution + 1,
		field->get_size() * resolution + 1,
	};
	auto vert_distance = 1.0f / resolution;

	// add vertices for the cells of the grid
	std::vector<float> verts{};
	auto vert_count = size[0] * size[1];
	verts.reserve(vert_count * 4);
	for (int i = 0; i < static_cast<int>(size[0]); ++i) {
		for (int j = 0; j < static_cast<int>(size[1]); ++j) {
			// for each vertex, compare the surrounding tiles
			std::vector<float> surround{};
			if (j - 1 >= 0 and i - 1 >= 0) {
				auto cost = field->get_cost((i - 1) / resolution, (j - 1) / resolution);
				surround.push_back(cost);
			}
			if (j < static_cast<int>(field->get_size()) and i - 1 >= 0) {
				auto cost = field->get_cost((i - 1) / resolution, j / resolution);
				surround.push_back(cost);
			}
			if (j < static_cast<int>(field->get_size()) and i < static_cast<int>(field->get_size())) {
				auto cost = field->get_cost(i / resolution, j / resolution);
				surround.push_back(cost);
			}
			if (j - 1 >= 0 and i < static_cast<int>(field->get_size())) {
				auto cost = field->get_cost(i / resolution, (j - 1) / resolution);
				surround.push_back(cost);
			}
			// use the cost of the most expensive surrounding tile
			auto max_cost = *std::max_element(surround.begin(), surround.end());

			coord::scene3 v{
				static_cast<float>(i * vert_distance),
				static_cast<float>(j * vert_distance),
				0,
			};
			auto world_v = v.to_world_space();
			verts.push_back(world_v[0]);
			verts.push_back(world_v[1]);
			verts.push_back(world_v[2]);
			verts.push_back(max_cost);
		}
	}

	// split the grid into triangles using an index array
	std::vector<uint16_t> idxs;
	idxs.reserve((size[0] - 1) * (size[1] - 1) * 6);
	// iterate over all tiles in the grid by columns, i.e. starting
	// from the left corner to the bottom corner if you imagine it from
	// the camera's point of view
	for (size_t i = 0; i < size[0] - 1; ++i) {
		for (size_t j = 0; j < size[1] - 1; ++j) {
			// since we are working on tiles, we split each tile into two triangles
			// with counter-clockwise vertex order
			idxs.push_back(j + i * size[1]);               // bottom left
			idxs.push_back(j + 1 + i * size[1]);           // bottom right
			idxs.push_back(j + size[1] + i * size[1]);     // top left
			idxs.push_back(j + 1 + i * size[1]);           // bottom right
			idxs.push_back(j + size[1] + 1 + i * size[1]); // top right
			idxs.push_back(j + size[1] + i * size[1]);     // top left
		}
	}

	renderer::resources::VertexInputInfo info{
		{renderer::resources::vertex_input_t::V3F32, renderer::resources::vertex_input_t::F32},
		renderer::resources::vertex_layout_t::AOS,
		renderer::resources::vertex_primitive_t::TRIANGLES,
		renderer::resources::index_t::U16};

	auto const vert_data_size = verts.size() * sizeof(float);
	std::vector<uint8_t> vert_data(vert_data_size);
	std::memcpy(vert_data.data(), verts.data(), vert_data_size);

	auto const idx_data_size = idxs.size() * sizeof(uint16_t);
	std::vector<uint8_t> idx_data(idx_data_size);
	std::memcpy(idx_data.data(), idxs.data(), idx_data_size);

	return {std::move(vert_data), std::move(idx_data), info};
}

renderer::resources::MeshData get_integration_field_mesh(const std::shared_ptr<IntegrationField> &field,
                                                         size_t resolution = 2) {
	// increase by 1 in every dimension because to get the vertex length
	// of each dimension
	util::Vector2s size{
		field->get_size() * resolution + 1,
		field->get_size() * resolution + 1,
	};
	auto vert_distance = 1.0f / resolution;

	// add vertices for the cells of the grid
	std::vector<float> verts{};
	auto vert_count = size[0] * size[1];
	verts.reserve(vert_count * 4);
	for (int i = 0; i < static_cast<int>(size[0]); ++i) {
		for (int j = 0; j < static_cast<int>(size[1]); ++j) {
			// for each vertex, compare the surrounding tiles
			std::vector<float> surround{};
			if (j - 1 >= 0 and i - 1 >= 0) {
				auto cost = field->get_cell((i - 1) / resolution, (j - 1) / resolution);
				surround.push_back(cost);
			}
			if (j < static_cast<int>(field->get_size()) and i - 1 >= 0) {
				auto cost = field->get_cell((i - 1) / resolution, j / resolution);
				surround.push_back(cost);
			}
			if (j < static_cast<int>(field->get_size()) and i < static_cast<int>(field->get_size())) {
				auto cost = field->get_cell(i / resolution, j / resolution);
				surround.push_back(cost);
			}
			if (j - 1 >= 0 and i < static_cast<int>(field->get_size())) {
				auto cost = field->get_cell(i / resolution, (j - 1) / resolution);
				surround.push_back(cost);
			}
			// use the cost of the most expensive surrounding tile
			auto max_cost = *std::max_element(surround.begin(), surround.end());

			coord::scene3 v{
				static_cast<float>(i * vert_distance),
				static_cast<float>(j * vert_distance),
				0,
			};
			auto world_v = v.to_world_space();
			verts.push_back(world_v[0]);
			verts.push_back(world_v[1]);
			verts.push_back(world_v[2]);
			verts.push_back(max_cost);
		}
	}

	// split the grid into triangles using an index array
	std::vector<uint16_t> idxs;
	idxs.reserve((size[0] - 1) * (size[1] - 1) * 6);
	// iterate over all tiles in the grid by columns, i.e. starting
	// from the left corner to the bottom corner if you imagine it from
	// the camera's point of view
	for (size_t i = 0; i < size[0] - 1; ++i) {
		for (size_t j = 0; j < size[1] - 1; ++j) {
			// since we are working on tiles, we split each tile into two triangles
			// with counter-clockwise vertex order
			idxs.push_back(j + i * size[1]);               // bottom left
			idxs.push_back(j + 1 + i * size[1]);           // bottom right
			idxs.push_back(j + size[1] + i * size[1]);     // top left
			idxs.push_back(j + 1 + i * size[1]);           // bottom right
			idxs.push_back(j + size[1] + 1 + i * size[1]); // top right
			idxs.push_back(j + size[1] + i * size[1]);     // top left
		}
	}

	renderer::resources::VertexInputInfo info{
		{renderer::resources::vertex_input_t::V3F32, renderer::resources::vertex_input_t::F32},
		renderer::resources::vertex_layout_t::AOS,
		renderer::resources::vertex_primitive_t::TRIANGLES,
		renderer::resources::index_t::U16};

	auto const vert_data_size = verts.size() * sizeof(float);
	std::vector<uint8_t> vert_data(vert_data_size);
	std::memcpy(vert_data.data(), verts.data(), vert_data_size);

	auto const idx_data_size = idxs.size() * sizeof(uint16_t);
	std::vector<uint8_t> idx_data(idx_data_size);
	std::memcpy(idx_data.data(), idxs.data(), idx_data_size);

	return {std::move(vert_data), std::move(idx_data), info};
}

/**
 * Create a mesh for the grid.
 *
 * @param side_length Length of the grid's side.
 *
 * @return Mesh data for the grid.
 */
renderer::resources::MeshData get_grid_mesh(size_t side_length) {
	// increase by 1 in every dimension because to get the vertex length
	// of each dimension
	util::Vector2s size{side_length + 1, side_length + 1};

	// add vertices for the cells of the grid
	std::vector<float> verts{};
	auto vert_count = size[0] * size[1];
	verts.reserve(vert_count * 3);
	for (int i = 0; i < (int)size[0]; ++i) {
		for (int j = 0; j < (int)size[1]; ++j) {
			coord::scene3 v{
				static_cast<float>(i),
				static_cast<float>(j),
				0,
			};
			auto world_v = v.to_world_space();
			verts.push_back(world_v[0]);
			verts.push_back(world_v[1]);
			verts.push_back(world_v[2]);
		}
	}

	// split the grid into lines using an index array
	std::vector<uint16_t> idxs;
	idxs.reserve((size[0] - 1) * (size[1] - 1) * 8);
	// iterate over all tiles in the grid by columns, i.e. starting
	// from the left corner to the bottom corner if you imagine it from
	// the camera's point of view
	for (size_t i = 0; i < size[0] - 1; ++i) {
		for (size_t j = 0; j < size[1] - 1; ++j) {
			// since we are working on tiles, we just draw a square using the 4 vertices
			idxs.push_back(j + i * size[1]);               // bottom left
			idxs.push_back(j + 1 + i * size[1]);           // bottom right
			idxs.push_back(j + 1 + i * size[1]);           // bottom right
			idxs.push_back(j + size[1] + 1 + i * size[1]); // top right
			idxs.push_back(j + size[1] + 1 + i * size[1]); // top right
			idxs.push_back(j + size[1] + i * size[1]);     // top left
			idxs.push_back(j + size[1] + i * size[1]);     // top left
			idxs.push_back(j + i * size[1]);               // bottom left
		}
	}

	renderer::resources::VertexInputInfo info{
		{renderer::resources::vertex_input_t::V3F32},
		renderer::resources::vertex_layout_t::AOS,
		renderer::resources::vertex_primitive_t::LINES,
		renderer::resources::index_t::U16};

	auto const vert_data_size = verts.size() * sizeof(float);
	std::vector<uint8_t> vert_data(vert_data_size);
	std::memcpy(vert_data.data(), verts.data(), vert_data_size);

	auto const idx_data_size = idxs.size() * sizeof(uint16_t);
	std::vector<uint8_t> idx_data(idx_data_size);
	std::memcpy(idx_data.data(), idxs.data(), idx_data_size);

	return {std::move(vert_data), std::move(idx_data), info};
}

void path_demo_0(const util::Path &path) {
	auto qtapp = std::make_shared<renderer::gui::GuiApplicationWithLogger>();

	renderer::opengl::GlWindow window("openage pathfinding test", 1440, 720, true);
	auto renderer = window.make_renderer();

	// Camera for correct projection of terrain
	auto camera = std::make_shared<renderer::camera::Camera>(renderer, window.get_size());
	camera->look_at_coord({5, 5, 0});
	window.add_resize_callback([&](size_t w, size_t h, double /*scale*/) {
		camera->resize(w, h);
	});

	// Shader sources
	auto shaderdir = path / "assets" / "test" / "shaders" / "pathfinding";

	// Shader for rendering the cost field
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

	// Shader for rendering the integration field
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

	// Shader for rendering the flow field
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

	// Shader for rendering the grid
	auto grid_vshader_file = shaderdir / "demo_0_grid.vert.glsl";
	auto grid_vshader_src = renderer::resources::ShaderSource(
		renderer::resources::shader_lang_t::glsl,
		renderer::resources::shader_stage_t::vertex,
		grid_vshader_file);

	auto grid_fshader_file = shaderdir / "demo_0_grid.frag.glsl";
	auto grid_fshader_src = renderer::resources::ShaderSource(
		renderer::resources::shader_lang_t::glsl,
		renderer::resources::shader_stage_t::fragment,
		grid_fshader_file);

	// Shader for monocolored objects
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

	// Shader for rendering to the display target
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
	auto grid_shader = renderer->add_shader({grid_vshader_src, grid_fshader_src});
	auto obj_shader = renderer->add_shader({obj_vshader_src, obj_fshader_src});
	auto display_shader = renderer->add_shader({display_vshader_src, display_fshader_src});

	// Make a framebuffer for the background render pass to draw into
	auto size = window.get_size();
	auto background_texture = renderer->add_texture(
		renderer::resources::Texture2dInfo(size[0],
	                                       size[1],
	                                       renderer::resources::pixel_format::rgba8));
	auto depth_texture = renderer->add_texture(
		renderer::resources::Texture2dInfo(size[0],
	                                       size[1],
	                                       renderer::resources::pixel_format::depth24));
	auto fbo = renderer->create_texture_target({background_texture, depth_texture});
	auto background_pass = renderer->add_render_pass({}, fbo);

	// Make a framebuffer for the field render passes to draw into
	auto field_texture = renderer->add_texture(
		renderer::resources::Texture2dInfo(size[0],
	                                       size[1],
	                                       renderer::resources::pixel_format::rgba8));
	auto depth_texture_2 = renderer->add_texture(
		renderer::resources::Texture2dInfo(size[0],
	                                       size[1],
	                                       renderer::resources::pixel_format::depth24));
	auto field_fbo = renderer->create_texture_target({field_texture, depth_texture_2});
	auto field_pass = renderer->add_render_pass({}, field_fbo);

	// Make a framebuffer for the grid render passes to draw into
	auto grid_texture = renderer->add_texture(
		renderer::resources::Texture2dInfo(size[0],
	                                       size[1],
	                                       renderer::resources::pixel_format::rgba8));
	auto depth_texture_3 = renderer->add_texture(
		renderer::resources::Texture2dInfo(size[0],
	                                       size[1],
	                                       renderer::resources::pixel_format::depth24));
	auto grid_fbo = renderer->create_texture_target({grid_texture, depth_texture_3});
	auto grid_pass = renderer->add_render_pass({}, grid_fbo);

	// Make two objects that draw the results of the previous passes onto the screen
	// in the display render pass
	auto bg_texture_unif = display_shader->new_uniform_input("color_texture", background_texture);
	auto quad = renderer->add_mesh_geometry(renderer::resources::MeshData::make_quad());
	renderer::Renderable bg_pass_obj{
		bg_texture_unif,
		quad,
		true,
		true,
	};
	auto field_texture_unif = display_shader->new_uniform_input("color_texture", field_texture);
	renderer::Renderable field_pass_obj{
		field_texture_unif,
		quad,
		true,
		true,
	};
	auto grid_texture_unif = display_shader->new_uniform_input("color_texture", grid_texture);
	renderer::Renderable grid_pass_obj{
		grid_texture_unif,
		quad,
		true,
		true,
	};
	auto display_pass = renderer->add_render_pass(
		{bg_pass_obj, field_pass_obj, grid_pass_obj},
		renderer->get_display_target());

	// Background object for contrast between field and display
	auto background_unifs = obj_shader->new_uniform_input(
		"color",
		Eigen::Vector4f{64.0 / 256, 128.0 / 256, 196.0 / 256, 1.0});
	auto background = renderer->add_mesh_geometry(renderer::resources::MeshData::make_quad());
	renderer::Renderable background_obj{
		background_unifs,
		background,
		true,
		true,
	};
	background_pass->add_renderables(background_obj);

	// Create the pathfinding fields
	auto cost_field = std::make_shared<CostField>(10);
	cost_field->set_cost(0, 0, 255);
	cost_field->set_cost(1, 0, 254);
	cost_field->set_cost(4, 3, 128);

	auto integration_field = std::make_shared<IntegrationField>(10);
	integration_field->integrate(cost_field, 7, 7);
	auto flow_field = std::make_shared<FlowField>(10);

	// Create object for the cost field
	Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
	auto cost_field_unifs = cf_shader->new_uniform_input(
		"model",
		model,
		"view",
		camera->get_view_matrix(),
		"proj",
		camera->get_projection_matrix());
	auto cost_field_mesh = get_cost_field_mesh(cost_field);
	auto cost_field_geometry = renderer->add_mesh_geometry(cost_field_mesh);
	// auto cost_field_geometry = renderer->add_mesh_geometry(renderer::resources::MeshData::make_quad());
	renderer::Renderable cost_field_renderable{
		cost_field_unifs,
		cost_field_geometry,
		true,
		true,
	};
	field_pass->add_renderables(cost_field_renderable);

	// Create object for the integration field
	auto integration_field_unifs = if_shader->new_uniform_input(
		"model",
		model,
		"view",
		camera->get_view_matrix(),
		"proj",
		camera->get_projection_matrix());
	auto integration_field_mesh = get_integration_field_mesh(integration_field);
	auto integration_field_geometry = renderer->add_mesh_geometry(integration_field_mesh);
	renderer::Renderable integration_field_renderable{
		integration_field_unifs,
		integration_field_geometry,
		true,
		true,
	};
	field_pass->add_renderables(integration_field_renderable);

	// Create object for the grid
	auto grid_unifs = grid_shader->new_uniform_input(
		"model",
		model,
		"view",
		camera->get_view_matrix(),
		"proj",
		camera->get_projection_matrix());
	auto grid_mesh = get_grid_mesh(10);
	auto grid_geometry = renderer->add_mesh_geometry(grid_mesh);
	renderer::Renderable grid_renderable{
		grid_unifs,
		grid_geometry,
		true,
		true,
	};
	grid_pass->add_renderables(grid_renderable);

	while (not window.should_close()) {
		qtapp->process_events();

		renderer->render(background_pass);
		renderer->render(field_pass);
		renderer->render(grid_pass);
		renderer->render(display_pass);

		window.update();

		renderer->check_error();
	}
	window.close();
}

} // namespace openage::path::tests
