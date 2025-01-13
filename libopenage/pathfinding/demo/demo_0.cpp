// Copyright 2024-2025 the openage authors. See copying.md for legal info.

#include "demo_0.h"

#include <QKeyEvent>
#include <QMouseEvent>

#include "coord/tile.h"
#include "pathfinding/cost_field.h"
#include "pathfinding/flow_field.h"
#include "pathfinding/integration_field.h"
#include "renderer/camera/camera.h"
#include "renderer/camera/definitions.h"
#include "renderer/gui/integration/public/gui_application_with_logger.h"
#include "renderer/opengl/window.h"
#include "renderer/render_pass.h"
#include "renderer/renderable.h"
#include "renderer/renderer.h"
#include "renderer/resources/mesh_data.h"
#include "renderer/resources/shader_source.h"
#include "renderer/resources/texture_info.h"
#include "renderer/shader_program.h"
#include "util/math_constants.h"
#include "util/vector.h"


namespace openage::path::tests {

void path_demo_0(const util::Path &path) {
	// Side length of the field
	// Creates a 10x10 grid
	auto field_length = 10;

	// Cost field with some obstacles
	auto cost_field = std::make_shared<CostField>(field_length);

	const time::time_t time = time::TIME_ZERO;
	cost_field->set_cost(coord::tile_delta{0, 0}, COST_IMPASSABLE, time);
	cost_field->set_cost(coord::tile_delta{1, 0}, 254, time);
	cost_field->set_cost(coord::tile_delta{4, 3}, 128, time);
	cost_field->set_cost(coord::tile_delta{5, 3}, 128, time);
	cost_field->set_cost(coord::tile_delta{6, 3}, 128, time);
	cost_field->set_cost(coord::tile_delta{4, 4}, 128, time);
	cost_field->set_cost(coord::tile_delta{5, 4}, 128, time);
	cost_field->set_cost(coord::tile_delta{6, 4}, 128, time);
	cost_field->set_cost(coord::tile_delta{1, 7}, COST_IMPASSABLE, time);
	log::log(INFO << "Created cost field");

	// Create an integration field from the cost field
	auto integration_field = std::make_shared<IntegrationField>(field_length);
	log::log(INFO << "Created integration field");

	// Set cell (7, 7) to be the initial target cell
	auto wavefront_blocked = integration_field->integrate_los(cost_field, coord::tile_delta{7, 7});
	integration_field->integrate_cost(cost_field, std::move(wavefront_blocked));
	log::log(INFO << "Calculated integration field for target cell (7, 7)");

	// Create a flow field from the integration field
	auto flow_field = std::make_shared<FlowField>(field_length);
	log::log(INFO << "Created flow field");

	flow_field->build(integration_field);
	log::log(INFO << "Built flow field from integration field");

	// Render the grid and the pathfinding results
	auto qtapp = std::make_shared<renderer::gui::GuiApplicationWithLogger>();

	// Create a window for rendering
	renderer::window_settings settings;
	settings.width = 1440;
	settings.height = 720;
	auto window = std::make_shared<renderer::opengl::GlWindow>("openage pathfinding test", settings);
	auto render_manager = std::make_shared<RenderManager0>(qtapp, window, path);
	log::log(INFO << "Created render manager for pathfinding demo");

	// Show the cost field on startup
	render_manager->show_cost_field(cost_field);
	auto current_field = RenderManager0::field_t::COST;
	log::log(INFO << "Showing cost field");

	// Make steering vector visibility toggleable
	auto vectors_visible = false;

	// Enable mouse button callbacks
	window->add_mouse_button_callback([&](const QMouseEvent &ev) {
		if (ev.type() == QEvent::MouseButtonRelease) {
			if (ev.button() == Qt::RightButton) { // Set target cell
				auto tile_pos = render_manager->select_tile(ev.position().x(), ev.position().y());
				auto grid_x = tile_pos.first;
				auto grid_y = tile_pos.second;

				if (grid_x >= 0 and grid_x < field_length and grid_y >= 0 and grid_y < field_length) {
					log::log(INFO << "Selected new target cell (" << grid_x << ", " << grid_y << ")");

					// Recalculate the integration field and the flow field
					integration_field->reset();
					auto wavefront_blocked = integration_field->integrate_los(cost_field,
					                                                          coord::tile_delta{grid_x, grid_y});
					integration_field->integrate_cost(cost_field, std::move(wavefront_blocked));
					log::log(INFO << "Calculated integration field for target cell ("
					              << grid_x << ", " << grid_y << ")");

					flow_field->reset();
					flow_field->build(integration_field);
					log::log(INFO << "Built flow field from integration field");

					// Show the new field values and vectors
					switch (current_field) {
					case RenderManager0::field_t::COST:
						render_manager->show_cost_field(cost_field);
						break;
					case RenderManager0::field_t::INTEGRATION:
						render_manager->show_integration_field(integration_field);
						break;
					case RenderManager0::field_t::FLOW:
						render_manager->show_flow_field(flow_field, integration_field);
						break;
					}

					if (vectors_visible) {
						render_manager->show_vectors(flow_field);
					}
				}
			}
		}
	});

	// Enable key callbacks
	window->add_key_callback([&](const QKeyEvent &ev) {
		if (ev.type() == QEvent::KeyRelease) {
			if (ev.key() == Qt::Key_F1) { // Show cost field
				render_manager->show_cost_field(cost_field);
				current_field = RenderManager0::field_t::COST;
				log::log(INFO << "Showing cost field");
			}
			else if (ev.key() == Qt::Key_F2) { // Show integration field
				render_manager->show_integration_field(integration_field);
				current_field = RenderManager0::field_t::INTEGRATION;
				log::log(INFO << "Showing integration field");
			}
			else if (ev.key() == Qt::Key_F3) { // Show flow field
				render_manager->show_flow_field(flow_field, integration_field);
				current_field = RenderManager0::field_t::FLOW;
				log::log(INFO << "Showing flow field");
			}
			else if (ev.key() == Qt::Key_F4) { // Show steering vectors
				if (vectors_visible) {
					render_manager->hide_vectors();
					vectors_visible = false;
					log::log(INFO << "Hiding steering vectors");
				}
				else {
					render_manager->show_vectors(flow_field);
					vectors_visible = true;
					log::log(INFO << "Showing steering vectors");
				}
			}
		}
	});

	log::log(INFO << "Instructions:");
	log::log(INFO << "  1. Press F1/F2/F3 to show cost/integration/flow field");
	log::log(INFO << "  2. Press F4 to toggle steering vectors");

	// Run the render loop
	render_manager->run();
}


RenderManager0::RenderManager0(const std::shared_ptr<renderer::gui::GuiApplicationWithLogger> &app,
                               const std::shared_ptr<renderer::Window> &window,
                               const util::Path &path) :
	path{path},
	app{app},
	window{window},
	renderer{window->make_renderer()},
	camera{std::make_shared<renderer::camera::Camera>(renderer, window->get_size())} {
	// Position camera to look at center of the grid
	camera->look_at_coord({5, 5, 0});
	window->add_resize_callback([&](size_t w, size_t h, double /*scale*/) {
		camera->resize(w, h);
	});

	this->init_shaders();
	this->init_passes();
}


void RenderManager0::run() {
	while (not this->window->should_close()) {
		this->app->process_events();

		this->renderer->render(this->background_pass);
		this->renderer->render(this->field_pass);
		this->renderer->render(this->vector_pass);
		this->renderer->render(this->grid_pass);
		this->renderer->render(this->display_pass);

		this->window->update();

		this->renderer->check_error();
	}
	this->window->close();
}

void RenderManager0::show_cost_field(const std::shared_ptr<path::CostField> &field) {
	Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
	auto unifs = this->cost_shader->new_uniform_input(
		"model",
		model,
		"view",
		this->camera->get_view_matrix(),
		"proj",
		this->camera->get_projection_matrix());
	auto mesh = RenderManager0::get_cost_field_mesh(field);
	auto geometry = this->renderer->add_mesh_geometry(mesh);
	renderer::Renderable renderable{
		unifs,
		geometry,
		true,
		true,
	};
	this->field_pass->set_renderables({renderable});
}

void RenderManager0::show_integration_field(const std::shared_ptr<path::IntegrationField> &field) {
	Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
	auto unifs = this->integration_shader->new_uniform_input(
		"model",
		model,
		"view",
		this->camera->get_view_matrix(),
		"proj",
		this->camera->get_projection_matrix());
	auto mesh = get_integration_field_mesh(field, 4);
	auto geometry = this->renderer->add_mesh_geometry(mesh);
	renderer::Renderable renderable{
		unifs,
		geometry,
		true,
		true,
	};
	this->field_pass->set_renderables({renderable});
}

void RenderManager0::show_flow_field(const std::shared_ptr<path::FlowField> &flow_field,
                                     const std::shared_ptr<path::IntegrationField> &int_field) {
	Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
	auto unifs = this->flow_shader->new_uniform_input(
		"model",
		model,
		"view",
		this->camera->get_view_matrix(),
		"proj",
		this->camera->get_projection_matrix());
	auto mesh = get_flow_field_mesh(flow_field, int_field, 4);
	auto geometry = this->renderer->add_mesh_geometry(mesh);
	renderer::Renderable renderable{
		unifs,
		geometry,
		true,
		true,
	};
	this->field_pass->set_renderables({renderable});
}

void RenderManager0::show_vectors(const std::shared_ptr<path::FlowField> &field) {
	static const std::unordered_map<flow_dir_t, Eigen::Vector3f> offset_dir{
		{flow_dir_t::NORTH, {-0.25f, 0.0f, 0.0f}},
		{flow_dir_t::NORTH_EAST, {-0.25f, 0.0f, -0.25f}},
		{flow_dir_t::EAST, {0.0f, 0.0f, -0.25f}},
		{flow_dir_t::SOUTH_EAST, {0.25f, 0.0f, -0.25f}},
		{flow_dir_t::SOUTH, {0.25f, 0.0f, 0.0f}},
		{flow_dir_t::SOUTH_WEST, {0.25f, 0.0f, 0.25f}},
		{flow_dir_t::WEST, {0.0f, 0.0f, 0.25f}},
		{flow_dir_t::NORTH_WEST, {-0.25f, 0.0f, 0.25f}},
	};

	this->vector_pass->clear_renderables();
	for (size_t y = 0; y < field->get_size(); ++y) {
		for (size_t x = 0; x < field->get_size(); ++x) {
			auto cell = field->get_cell(coord::tile_delta(x, y));
			if (cell & FLOW_PATHABLE_MASK and not(cell & FLOW_LOS_MASK)) {
				Eigen::Affine3f arrow_model = Eigen::Affine3f::Identity();
				arrow_model.translate(Eigen::Vector3f(y + 0.5, 0, -1.0f * x - 0.5));
				auto dir = static_cast<flow_dir_t>(cell & FLOW_DIR_MASK);
				arrow_model.translate(offset_dir.at(dir));

				auto rotation_rad = (cell & FLOW_DIR_MASK) * -45 * math::DEGSPERRAD;
				arrow_model.rotate(Eigen::AngleAxisf(rotation_rad, Eigen::Vector3f::UnitY()));
				auto arrow_unifs = this->vector_shader->new_uniform_input(
					"model",
					arrow_model.matrix(),
					"view",
					camera->get_view_matrix(),
					"proj",
					camera->get_projection_matrix(),
					"color",
					Eigen::Vector4f{0.0f, 0.0f, 0.0f, 1.0f});
				auto arrow_mesh = get_arrow_mesh();
				auto arrow_geometry = renderer->add_mesh_geometry(arrow_mesh);
				renderer::Renderable arrow_renderable{
					arrow_unifs,
					arrow_geometry,
					true,
					true,
				};
				this->vector_pass->add_renderables(std::move(arrow_renderable));
			}
		}
	}
}

void RenderManager0::hide_vectors() {
	this->vector_pass->clear_renderables();
}

std::pair<int, int> RenderManager0::select_tile(double x, double y) {
	auto grid_plane_normal = Eigen::Vector3f{0, 1, 0};
	auto grid_plane_point = Eigen::Vector3f{0, 0, 0};
	auto camera_direction = renderer::camera::CAM_DIRECTION;
	auto camera_position = camera->get_input_pos(
		coord::input(x, y));

	Eigen::Vector3f intersect = camera_position + camera_direction * (grid_plane_point - camera_position).dot(grid_plane_normal) / camera_direction.dot(grid_plane_normal);
	auto grid_x = static_cast<int>(-1 * intersect[2]);
	auto grid_y = static_cast<int>(intersect[0]);

	return {grid_x, grid_y};
}

void RenderManager0::init_shaders() {
	// Shader sources
	auto shaderdir = this->path / "assets" / "test" / "shaders" / "pathfinding";

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

	// Shader for rendering steering vectors
	auto vec_vshader_file = shaderdir / "demo_0_vector.vert.glsl";
	auto vec_vshader_src = renderer::resources::ShaderSource(
		renderer::resources::shader_lang_t::glsl,
		renderer::resources::shader_stage_t::vertex,
		vec_vshader_file);

	auto vec_fshader_file = shaderdir / "demo_0_vector.frag.glsl";
	auto vec_fshader_src = renderer::resources::ShaderSource(
		renderer::resources::shader_lang_t::glsl,
		renderer::resources::shader_stage_t::fragment,
		vec_fshader_file);

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

	// Shader for 2D monocolored objects
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
	this->cost_shader = renderer->add_shader({cf_vshader_src, cf_fshader_src});
	this->integration_shader = renderer->add_shader({if_vshader_src, if_fshader_src});
	this->flow_shader = renderer->add_shader({ff_vshader_src, ff_fshader_src});
	this->vector_shader = renderer->add_shader({vec_vshader_src, vec_fshader_src});
	this->grid_shader = renderer->add_shader({grid_vshader_src, grid_fshader_src});
	this->obj_shader = renderer->add_shader({obj_vshader_src, obj_fshader_src});
	this->display_shader = renderer->add_shader({display_vshader_src, display_fshader_src});
}

void RenderManager0::init_passes() {
	auto size = this->window->get_size();

	// Make a framebuffer for the background render pass to draw into
	auto background_texture = renderer->add_texture(
		renderer::resources::Texture2dInfo(size[0],
	                                       size[1],
	                                       renderer::resources::pixel_format::rgba8));
	auto depth_texture = renderer->add_texture(
		renderer::resources::Texture2dInfo(size[0],
	                                       size[1],
	                                       renderer::resources::pixel_format::depth24));
	auto fbo = renderer->create_texture_target({background_texture, depth_texture});

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
	this->background_pass = renderer->add_render_pass({background_obj}, fbo);

	// Make a framebuffer for the field render pass to draw into
	auto field_texture = renderer->add_texture(
		renderer::resources::Texture2dInfo(size[0],
	                                       size[1],
	                                       renderer::resources::pixel_format::rgba8));
	auto depth_texture_2 = renderer->add_texture(
		renderer::resources::Texture2dInfo(size[0],
	                                       size[1],
	                                       renderer::resources::pixel_format::depth24));
	auto field_fbo = renderer->create_texture_target({field_texture, depth_texture_2});
	this->field_pass = renderer->add_render_pass({}, field_fbo);

	// Make a framebuffer for the vector render passes to draw into
	auto vector_texture = renderer->add_texture(
		renderer::resources::Texture2dInfo(size[0],
	                                       size[1],
	                                       renderer::resources::pixel_format::rgba8));
	auto depth_texture_3 = renderer->add_texture(
		renderer::resources::Texture2dInfo(size[0],
	                                       size[1],
	                                       renderer::resources::pixel_format::depth24));
	auto vector_fbo = renderer->create_texture_target({vector_texture, depth_texture_3});
	this->vector_pass = renderer->add_render_pass({}, vector_fbo);

	// Make a framebuffer for the grid render passes to draw into
	auto grid_texture = renderer->add_texture(
		renderer::resources::Texture2dInfo(size[0],
	                                       size[1],
	                                       renderer::resources::pixel_format::rgba8));
	auto depth_texture_4 = renderer->add_texture(
		renderer::resources::Texture2dInfo(size[0],
	                                       size[1],
	                                       renderer::resources::pixel_format::depth24));
	auto grid_fbo = renderer->create_texture_target({grid_texture, depth_texture_4});

	// Create object for the grid
	Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
	auto grid_unifs = grid_shader->new_uniform_input(
		"model",
		model,
		"view",
		camera->get_view_matrix(),
		"proj",
		camera->get_projection_matrix());
	auto grid_mesh = this->get_grid_mesh(10);
	auto grid_geometry = renderer->add_mesh_geometry(grid_mesh);
	renderer::Renderable grid_obj{
		grid_unifs,
		grid_geometry,
		true,
		true,
	};
	this->grid_pass = renderer->add_render_pass({grid_obj}, grid_fbo);

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
	auto vector_texture_unif = display_shader->new_uniform_input("color_texture", vector_texture);
	renderer::Renderable vector_pass_obj{
		vector_texture_unif,
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
	this->display_pass = renderer->add_render_pass(
		{bg_pass_obj, field_pass_obj, vector_pass_obj, grid_pass_obj},
		renderer->get_display_target());
}

renderer::resources::MeshData RenderManager0::get_cost_field_mesh(const std::shared_ptr<CostField> &field,
                                                                  size_t resolution) {
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

renderer::resources::MeshData RenderManager0::get_integration_field_mesh(const std::shared_ptr<IntegrationField> &field,
                                                                         size_t resolution) {
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
				auto cost = field->get_cell((i - 1) / resolution, (j - 1) / resolution).cost;
				surround.push_back(cost);
			}
			if (j < static_cast<int>(field->get_size()) and i - 1 >= 0) {
				auto cost = field->get_cell((i - 1) / resolution, j / resolution).cost;
				surround.push_back(cost);
			}
			if (j < static_cast<int>(field->get_size()) and i < static_cast<int>(field->get_size())) {
				auto cost = field->get_cell(i / resolution, j / resolution).cost;
				surround.push_back(cost);
			}
			if (j - 1 >= 0 and i < static_cast<int>(field->get_size())) {
				auto cost = field->get_cell(i / resolution, (j - 1) / resolution).cost;
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

renderer::resources::MeshData RenderManager0::get_flow_field_mesh(const std::shared_ptr<FlowField> &flow_field,
                                                                  const std::shared_ptr<path::IntegrationField> &int_field,
                                                                  size_t resolution) {
	// increase by 1 in every dimension because to get the vertex length
	// of each dimension
	util::Vector2s size{
		flow_field->get_size() * resolution + 1,
		flow_field->get_size() * resolution + 1,
	};
	auto vert_distance = 1.0f / resolution;

	// add vertices for the cells of the grid
	std::vector<float> verts{};
	auto vert_count = size[0] * size[1];
	verts.reserve(vert_count * 5);
	for (int i = 0; i < static_cast<int>(size[0]); ++i) {
		for (int j = 0; j < static_cast<int>(size[1]); ++j) {
			// for each vertex, compare the surrounding tiles
			std::vector<flow_t> ff_surround{};
			std::vector<integrated_flags_t> int_surround{};
			if (j - 1 >= 0 and i - 1 >= 0) {
				auto ff_cost = flow_field->get_cell((i - 1) / resolution, (j - 1) / resolution);
				ff_surround.push_back(ff_cost);

				auto int_flags = int_field->get_cell((i - 1) / resolution, (j - 1) / resolution).flags;
				int_surround.push_back(int_flags);
			}
			if (j < static_cast<int>(flow_field->get_size()) and i - 1 >= 0) {
				auto ff_cost = flow_field->get_cell((i - 1) / resolution, j / resolution);
				ff_surround.push_back(ff_cost);

				auto int_flags = int_field->get_cell((i - 1) / resolution, j / resolution).flags;
				int_surround.push_back(int_flags);
			}
			if (j < static_cast<int>(flow_field->get_size()) and i < static_cast<int>(flow_field->get_size())) {
				auto ff_cost = flow_field->get_cell(i / resolution, j / resolution);
				ff_surround.push_back(ff_cost);

				auto int_flags = int_field->get_cell(i / resolution, j / resolution).flags;
				int_surround.push_back(int_flags);
			}
			if (j - 1 >= 0 and i < static_cast<int>(flow_field->get_size())) {
				auto ff_cost = flow_field->get_cell(i / resolution, (j - 1) / resolution);
				ff_surround.push_back(ff_cost);

				auto int_flags = int_field->get_cell(i / resolution, (j - 1) / resolution).flags;
				int_surround.push_back(int_flags);
			}
			// combine the flags of the sorrounding tiles
			auto ff_max_flags = 0;
			for (auto &val : ff_surround) {
				ff_max_flags |= val & 0xF0;
			}
			auto int_max_flags = 0;
			for (auto &val : int_surround) {
				int_max_flags |= val;
			}

			coord::scene3 v{
				static_cast<float>(i * vert_distance),
				static_cast<float>(j * vert_distance),
				0,
			};
			auto world_v = v.to_world_space();
			verts.push_back(world_v[0]);
			verts.push_back(world_v[1]);
			verts.push_back(world_v[2]);
			verts.push_back(ff_max_flags);
			verts.push_back(int_max_flags);
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
		{renderer::resources::vertex_input_t::V3F32,
	     renderer::resources::vertex_input_t::F32,
	     renderer::resources::vertex_input_t::F32},
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

renderer::resources::MeshData RenderManager0::get_arrow_mesh() {
	// vertices for the arrow
	// x, y, z
	std::vector<float> verts{
		// clang-format off
         0.2f, 0.01f, -0.05f,
         0.2f, 0.01f,  0.05f,
        -0.2f, 0.01f, -0.05f,
        -0.2f, 0.01f,  0.05f,
		// clang-format on
	};

	renderer::resources::VertexInputInfo info{
		{renderer::resources::vertex_input_t::V3F32},
		renderer::resources::vertex_layout_t::AOS,
		renderer::resources::vertex_primitive_t::TRIANGLE_STRIP};

	auto const vert_data_size = verts.size() * sizeof(float);
	std::vector<uint8_t> vert_data(vert_data_size);
	std::memcpy(vert_data.data(), verts.data(), vert_data_size);

	return {std::move(vert_data), info};
}

renderer::resources::MeshData RenderManager0::get_grid_mesh(size_t side_length) {
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


} // namespace openage::path::tests
