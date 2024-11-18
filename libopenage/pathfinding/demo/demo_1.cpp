// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "demo_1.h"

#include <QMouseEvent>

#include "pathfinding/cost_field.h"
#include "pathfinding/grid.h"
#include "pathfinding/path.h"
#include "pathfinding/pathfinder.h"
#include "pathfinding/portal.h"
#include "pathfinding/sector.h"
#include "util/timer.h"

#include "renderer/gui/integration/public/gui_application_with_logger.h"
#include "renderer/opengl/window.h"
#include "renderer/render_pass.h"
#include "renderer/renderable.h"
#include "renderer/resources/shader_source.h"
#include "renderer/resources/texture_info.h"
#include "renderer/shader_program.h"
#include "renderer/window.h"


namespace openage::path::tests {

void path_demo_1(const util::Path &path) {
	auto grid = std::make_shared<Grid>(0, util::Vector2s{4, 3}, 10);

	// Initialize the cost field for each sector.
	for (auto &sector : grid->get_sectors()) {
		auto cost_field = sector->get_cost_field();
		std::vector<cost_t> sector_cost = sectors_cost.at(sector->get_id());
		cost_field->set_costs(std::move(sector_cost));
	}

	// Initialize portals between sectors.
	util::Vector2s grid_size = grid->get_size();
	portal_id_t portal_id = 0;
	for (size_t y = 0; y < grid_size[1]; y++) {
		for (size_t x = 0; x < grid_size[0]; x++) {
			auto sector = grid->get_sector(x, y);

			if (x < grid_size[0] - 1) {
				auto neighbor = grid->get_sector(x + 1, y);
				auto portals = sector->find_portals(neighbor, PortalDirection::EAST_WEST, portal_id);
				for (auto &portal : portals) {
					sector->add_portal(portal);
					neighbor->add_portal(portal);
				}
				portal_id += portals.size();
			}
			if (y < grid_size[1] - 1) {
				auto neighbor = grid->get_sector(x, y + 1);
				auto portals = sector->find_portals(neighbor, PortalDirection::NORTH_SOUTH, portal_id);
				for (auto &portal : portals) {
					sector->add_portal(portal);
					neighbor->add_portal(portal);
				}
				portal_id += portals.size();
			}
		}
	}

	// Connect portals inside sectors.
	for (auto sector : grid->get_sectors()) {
		sector->connect_exits();

		log::log(MSG(info) << "Sector " << sector->get_id() << " has " << sector->get_portals().size() << " portals.");
		for (auto portal : sector->get_portals()) {
			log::log(MSG(info) << "  Portal " << portal->get_id() << ":");
			log::log(MSG(info) << "    Connects sectors:  " << sector->get_id() << " to "
			                   << portal->get_exit_sector(sector->get_id()));
			log::log(MSG(info) << "    Entry start:       " << portal->get_entry_start(sector->get_id()));
			log::log(MSG(info) << "    Entry end:         " << portal->get_entry_end(sector->get_id()));
			log::log(MSG(info) << "    Connected portals: " << portal->get_connected(sector->get_id()).size());
		}
	}

	// Create a pathfinder for searching paths on the grid
	auto pathfinder = std::make_shared<path::Pathfinder>();
	pathfinder->add_grid(grid);

	util::Timer timer;

	// Create a path request and get the path
	coord::tile start{2, 26};
	coord::tile target{36, 2};

	PathRequest path_request{
		grid->get_id(),
		start,
		target,
	};
	grid->init_portal_nodes();
	timer.start();
	Path path_result = pathfinder->get_path(path_request);
	timer.stop();

	log::log(INFO << "Pathfinding took " << timer.getval() / 1000 << " us");

	// Create a renderer to display the grid and path
	auto qtapp = std::make_shared<renderer::gui::GuiApplicationWithLogger>();

	renderer::window_settings settings;
	settings.width = 1024;
	settings.height = 768;
	auto window = std::make_shared<renderer::opengl::GlWindow>("openage pathfinding test", settings);
	auto render_manager = std::make_shared<RenderManager1>(qtapp, window, path, grid);
	log::log(INFO << "Created render manager for pathfinding demo");

	window->add_mouse_button_callback([&](const QMouseEvent &ev) {
		if (ev.type() == QEvent::MouseButtonRelease) {
			auto cell_count_x = grid->get_size()[0] * grid->get_sector_size();
			auto cell_count_y = grid->get_size()[1] * grid->get_sector_size();
			auto window_size = window->get_size();

			double cell_size_x = static_cast<double>(window_size[0]) / cell_count_x;
			double cell_size_y = static_cast<double>(window_size[1]) / cell_count_y;

			coord::tile_t grid_x = ev.position().x() / cell_size_x;
			coord::tile_t grid_y = ev.position().y() / cell_size_y;

			if (ev.button() == Qt::RightButton) { // Set target cell
				target = coord::tile{grid_x, grid_y};
				PathRequest new_path_request{
					grid->get_id(),
					start,
					target,
				};

				timer.reset();
				timer.start();
				path_result = pathfinder->get_path(new_path_request);
				timer.stop();

				log::log(INFO << "Pathfinding took " << timer.getval() / 1000 << " us");

				if (path_result.status == PathResult::FOUND) {
					// Create renderables for the waypoints of the path
					render_manager->create_waypoint_tiles(path_result);
				}
			}
			else if (ev.button() == Qt::LeftButton) { // Set start cell
				start = coord::tile{grid_x, grid_y};
				PathRequest new_path_request{
					grid->get_id(),
					start,
					target,
				};

				timer.reset();
				timer.start();
				path_result = pathfinder->get_path(new_path_request);
				timer.stop();

				log::log(INFO << "Pathfinding took " << timer.getval() / 1000 << " us");

				if (path_result.status == PathResult::FOUND) {
					// Create renderables for the waypoints of the path
					render_manager->create_waypoint_tiles(path_result);
				}
			}
		}
	});

	// Create renderables for the waypoints of the path
	render_manager->create_waypoint_tiles(path_result);

	// Run the renderer pss to draw the grid and path into a window
	render_manager->run();
}


RenderManager1::RenderManager1(const std::shared_ptr<renderer::gui::GuiApplicationWithLogger> &app,
                               const std::shared_ptr<renderer::Window> &window,
                               const util::Path &path,
                               const std::shared_ptr<path::Grid> &grid) :
	path{path},
	grid{grid},
	app{app},
	window{window},
	renderer{window->make_renderer()} {
	this->init_shaders();
	this->init_passes();
}

void RenderManager1::run() {
	while (not this->window->should_close()) {
		this->app->process_events();

		this->renderer->render(this->background_pass);
		this->renderer->render(this->field_pass);
		this->renderer->render(this->waypoint_pass);
		this->renderer->render(this->grid_pass);
		this->renderer->render(this->display_pass);

		this->window->update();

		this->renderer->check_error();
	}
	this->window->close();
}

void RenderManager1::init_passes() {
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
	Eigen::Matrix4f id_matrix = Eigen::Matrix4f::Identity();
	auto background_unifs = obj_shader->new_uniform_input(
		"color",
		Eigen::Vector4f{1.0, 1.0, 1.0, 1.0},
		"model",
		id_matrix,
		"view",
		id_matrix,
		"proj",
		id_matrix);
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
	this->create_impassible_tiles(this->grid);
	this->create_portal_tiles(this->grid);

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

	// Make a framebuffer for the waypoint render pass to draw into
	auto waypoint_texture = renderer->add_texture(
		renderer::resources::Texture2dInfo(size[0],
	                                       size[1],
	                                       renderer::resources::pixel_format::rgba8));
	auto depth_texture_4 = renderer->add_texture(
		renderer::resources::Texture2dInfo(size[0],
	                                       size[1],
	                                       renderer::resources::pixel_format::depth24));
	auto waypoint_fbo = renderer->create_texture_target({waypoint_texture, depth_texture_4});
	this->waypoint_pass = renderer->add_render_pass({}, waypoint_fbo);

	// Create object for the grid
	auto model = Eigen::Affine3f::Identity();
	model.prescale(Eigen::Vector3f{
		2.0f / (this->grid->get_size()[0] * this->grid->get_sector_size()),
		2.0f / (this->grid->get_size()[1] * this->grid->get_sector_size()),
		1.0f});
	model.pretranslate(Eigen::Vector3f{-1.0f, -1.0f, 0.0f});
	auto grid_unifs = grid_shader->new_uniform_input(
		"model",
		model.matrix(),
		"view",
		id_matrix,
		"proj",
		id_matrix);
	auto grid_mesh = this->get_grid_mesh(this->grid);
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
	auto waypoint_texture_unif = display_shader->new_uniform_input("color_texture", waypoint_texture);
	renderer::Renderable waypoint_pass_obj{
		waypoint_texture_unif,
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
		{bg_pass_obj, field_pass_obj, waypoint_pass_obj, grid_pass_obj},
		renderer->get_display_target());
}

void RenderManager1::init_shaders() {
	// Shader sources
	auto shaderdir = this->path / "assets" / "test" / "shaders" / "pathfinding";

	// Shader for rendering the grid
	auto grid_vshader_file = shaderdir / "demo_1_grid.vert.glsl";
	auto grid_vshader_src = renderer::resources::ShaderSource(
		renderer::resources::shader_lang_t::glsl,
		renderer::resources::shader_stage_t::vertex,
		grid_vshader_file);

	auto grid_fshader_file = shaderdir / "demo_1_grid.frag.glsl";
	auto grid_fshader_src = renderer::resources::ShaderSource(
		renderer::resources::shader_lang_t::glsl,
		renderer::resources::shader_stage_t::fragment,
		grid_fshader_file);

	// Shader for 2D monocolored objects
	auto obj_vshader_file = shaderdir / "demo_1_obj.vert.glsl";
	auto obj_vshader_src = renderer::resources::ShaderSource(
		renderer::resources::shader_lang_t::glsl,
		renderer::resources::shader_stage_t::vertex,
		obj_vshader_file);

	auto obj_fshader_file = shaderdir / "demo_1_obj.frag.glsl";
	auto obj_fshader_src = renderer::resources::ShaderSource(
		renderer::resources::shader_lang_t::glsl,
		renderer::resources::shader_stage_t::fragment,
		obj_fshader_file);

	// Shader for rendering to the display target
	auto display_vshader_file = shaderdir / "demo_1_display.vert.glsl";
	auto display_vshader_src = renderer::resources::ShaderSource(
		renderer::resources::shader_lang_t::glsl,
		renderer::resources::shader_stage_t::vertex,
		display_vshader_file);

	auto display_fshader_file = shaderdir / "demo_1_display.frag.glsl";
	auto display_fshader_src = renderer::resources::ShaderSource(
		renderer::resources::shader_lang_t::glsl,
		renderer::resources::shader_stage_t::fragment,
		display_fshader_file);

	// Create the shaders
	this->grid_shader = renderer->add_shader({grid_vshader_src, grid_fshader_src});
	this->obj_shader = renderer->add_shader({obj_vshader_src, obj_fshader_src});
	this->display_shader = renderer->add_shader({display_vshader_src, display_fshader_src});
}


renderer::resources::MeshData RenderManager1::get_grid_mesh(const std::shared_ptr<path::Grid> &grid) {
	// increase by 1 in every dimension because to get the vertex length
	// of each dimension
	util::Vector2s size{
		grid->get_size()[0] * grid->get_sector_size() + 1,
		grid->get_size()[1] * grid->get_sector_size() + 1};

	// add vertices for the cells of the grid
	std::vector<float> verts{};
	auto vert_count = size[0] * size[1];
	verts.reserve(vert_count * 2);
	for (int i = 0; i < (int)size[0]; ++i) {
		for (int j = 0; j < (int)size[1]; ++j) {
			verts.push_back(i);
			verts.push_back(j);
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
		{renderer::resources::vertex_input_t::V2F32},
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

void RenderManager1::create_impassible_tiles(const std::shared_ptr<path::Grid> &grid) {
	auto width = grid->get_size()[0];
	auto height = grid->get_size()[1];
	auto sector_size = grid->get_sector_size();

	float tile_offset_width = 2.0f / (width * sector_size);
	float tile_offset_height = 2.0f / (height * sector_size);

	for (size_t sector_x = 0; sector_x < width; sector_x++) {
		for (size_t sector_y = 0; sector_y < height; sector_y++) {
			auto sector = grid->get_sector(sector_x, sector_y);
			auto cost_field = sector->get_cost_field();
			for (size_t y = 0; y < sector_size; y++) {
				for (size_t x = 0; x < sector_size; x++) {
					auto cost = cost_field->get_cost(x, y);
					if (cost == COST_IMPASSABLE) {
						std::array<float, 8> tile_data{
							-1.0f + x * tile_offset_width + sector_size * sector_x * tile_offset_width,
							1.0f - y * tile_offset_height - sector_size * sector_y * tile_offset_height,
							-1.0f + x * tile_offset_width + sector_size * sector_x * tile_offset_width,
							1.0f - (y + 1) * tile_offset_height - sector_size * sector_y * tile_offset_height,
							-1.0f + (x + 1) * tile_offset_width + sector_size * sector_x * tile_offset_width,
							1.0f - y * tile_offset_height - sector_size * sector_y * tile_offset_height,
							-1.0f + (x + 1) * tile_offset_width + sector_size * sector_x * tile_offset_width,
							1.0f - (y + 1) * tile_offset_height - sector_size * sector_y * tile_offset_height,
						};
						renderer::resources::VertexInputInfo info{
							{renderer::resources::vertex_input_t::V2F32},
							renderer::resources::vertex_layout_t::AOS,
							renderer::resources::vertex_primitive_t::TRIANGLE_STRIP};

						auto const data_size = tile_data.size() * sizeof(float);
						std::vector<uint8_t> verts(data_size);
						std::memcpy(verts.data(), tile_data.data(), data_size);

						auto tile_mesh = renderer::resources::MeshData(std::move(verts), info);
						auto tile_geometry = renderer->add_mesh_geometry(tile_mesh);

						Eigen::Matrix4f id_matrix = Eigen::Matrix4f::Identity();
						auto tile_unifs = obj_shader->new_uniform_input(
							"color",
							Eigen::Vector4f{0.0, 0.0, 0.0, 1.0},
							"model",
							id_matrix,
							"view",
							id_matrix,
							"proj",
							id_matrix);
						auto tile_obj = renderer::Renderable{
							tile_unifs,
							tile_geometry,
							true,
							true,
						};
						this->field_pass->add_renderables({tile_obj});
					}
				}
			}
		}
	}
}

void RenderManager1::create_portal_tiles(const std::shared_ptr<path::Grid> &grid) {
	auto width = grid->get_size()[0];
	auto height = grid->get_size()[1];
	auto sector_size = grid->get_sector_size();

	float tile_offset_width = 2.0f / (width * sector_size);
	float tile_offset_height = 2.0f / (height * sector_size);

	for (size_t sector_x = 0; sector_x < width; sector_x++) {
		for (size_t sector_y = 0; sector_y < height; sector_y++) {
			auto sector = grid->get_sector(sector_x, sector_y);

			for (auto &portal : sector->get_portals()) {
				auto start = portal->get_entry_start(sector->get_id());
				auto end = portal->get_entry_end(sector->get_id());
				auto direction = portal->get_direction();

				std::vector<coord::tile> tiles;
				if (direction == PortalDirection::NORTH_SOUTH) {
					auto y = start.se;
					for (auto x = start.ne; x <= end.ne; ++x) {
						tiles.push_back(coord::tile(x, y));
					}
				}
				else {
					auto x = start.ne;
					for (auto y = start.se; y <= end.se; ++y) {
						tiles.push_back(coord::tile(x, y));
					}
				}

				for (auto tile : tiles) {
					std::array<float, 8> tile_data{
						-1.0f + tile.ne * tile_offset_width + sector_size * sector_x * tile_offset_width,
						1.0f - tile.se * tile_offset_height - sector_size * sector_y * tile_offset_height,
						-1.0f + tile.ne * tile_offset_width + sector_size * sector_x * tile_offset_width,
						1.0f - (tile.se + 1) * tile_offset_height - sector_size * sector_y * tile_offset_height,
						-1.0f + (tile.ne + 1) * tile_offset_width + sector_size * sector_x * tile_offset_width,
						1.0f - tile.se * tile_offset_height - sector_size * sector_y * tile_offset_height,
						-1.0f + (tile.ne + 1) * tile_offset_width + sector_size * sector_x * tile_offset_width,
						1.0f - (tile.se + 1) * tile_offset_height - sector_size * sector_y * tile_offset_height,
					};
					renderer::resources::VertexInputInfo info{
						{renderer::resources::vertex_input_t::V2F32},
						renderer::resources::vertex_layout_t::AOS,
						renderer::resources::vertex_primitive_t::TRIANGLE_STRIP};

					auto const data_size = tile_data.size() * sizeof(float);
					std::vector<uint8_t> verts(data_size);
					std::memcpy(verts.data(), tile_data.data(), data_size);

					auto tile_mesh = renderer::resources::MeshData(std::move(verts), info);
					auto tile_geometry = renderer->add_mesh_geometry(tile_mesh);

					Eigen::Matrix4f id_matrix = Eigen::Matrix4f::Identity();
					auto tile_unifs = obj_shader->new_uniform_input(
						"color",
						Eigen::Vector4f{0.0, 0.0, 0.0, 0.3},
						"model",
						id_matrix,
						"view",
						id_matrix,
						"proj",
						id_matrix);
					auto tile_obj = renderer::Renderable{
						tile_unifs,
						tile_geometry,
						true,
						true,
					};
					this->field_pass->add_renderables({tile_obj});
				}
			}
		}
	}
}

void RenderManager1::create_waypoint_tiles(const Path &path) {
	auto width = grid->get_size()[0];
	auto height = grid->get_size()[1];
	auto sector_size = grid->get_sector_size();

	float tile_offset_width = 2.0f / (width * sector_size);
	float tile_offset_height = 2.0f / (height * sector_size);

	Eigen::Matrix4f id_matrix = Eigen::Matrix4f::Identity();

	this->waypoint_pass->clear_renderables();

	// Draw in-between waypoints
	for (size_t i = 1; i < path.waypoints.size() - 1; i++) {
		auto tile = path.waypoints[i];
		std::array<float, 8> tile_data{
			-1.0f + tile.ne * tile_offset_width,
			1.0f - tile.se * tile_offset_height,
			-1.0f + tile.ne * tile_offset_width,
			1.0f - (tile.se + 1) * tile_offset_height,
			-1.0f + (tile.ne + 1) * tile_offset_width,
			1.0f - tile.se * tile_offset_height,
			-1.0f + (tile.ne + 1) * tile_offset_width,
			1.0f - (tile.se + 1) * tile_offset_height,
		};

		renderer::resources::VertexInputInfo info{
			{renderer::resources::vertex_input_t::V2F32},
			renderer::resources::vertex_layout_t::AOS,
			renderer::resources::vertex_primitive_t::TRIANGLE_STRIP};

		auto const data_size = tile_data.size() * sizeof(float);
		std::vector<uint8_t> verts(data_size);
		std::memcpy(verts.data(), tile_data.data(), data_size);

		auto tile_mesh = renderer::resources::MeshData(std::move(verts), info);
		auto tile_geometry = renderer->add_mesh_geometry(tile_mesh);

		auto tile_unifs = obj_shader->new_uniform_input(
			"color",
			Eigen::Vector4f{0.0, 0.25, 1.0, 1.0},
			"model",
			id_matrix,
			"view",
			id_matrix,
			"proj",
			id_matrix);
		auto tile_obj = renderer::Renderable{
			tile_unifs,
			tile_geometry,
			true,
			true,
		};
		this->waypoint_pass->add_renderables({tile_obj});
	}

	// Draw start and end waypoints with different colors
	auto start_tile = path.waypoints.front();
	std::array<float, 8> start_tile_data{
		-1.0f + start_tile.ne * tile_offset_width,
		1.0f - start_tile.se * tile_offset_height,
		-1.0f + start_tile.ne * tile_offset_width,
		1.0f - (start_tile.se + 1) * tile_offset_height,
		-1.0f + (start_tile.ne + 1) * tile_offset_width,
		1.0f - start_tile.se * tile_offset_height,
		-1.0f + (start_tile.ne + 1) * tile_offset_width,
		1.0f - (start_tile.se + 1) * tile_offset_height,
	};

	renderer::resources::VertexInputInfo start_info{
		{renderer::resources::vertex_input_t::V2F32},
		renderer::resources::vertex_layout_t::AOS,
		renderer::resources::vertex_primitive_t::TRIANGLE_STRIP};

	auto const start_data_size = start_tile_data.size() * sizeof(float);
	std::vector<uint8_t> start_verts(start_data_size);
	std::memcpy(start_verts.data(), start_tile_data.data(), start_data_size);

	auto start_tile_mesh = renderer::resources::MeshData(std::move(start_verts), start_info);
	auto start_tile_geometry = renderer->add_mesh_geometry(start_tile_mesh);
	auto start_tile_unifs = obj_shader->new_uniform_input(
		"color",
		Eigen::Vector4f{0.0, 0.5, 0.0, 1.0},
		"model",
		id_matrix,
		"view",
		id_matrix,
		"proj",
		id_matrix);
	auto start_tile_obj = renderer::Renderable{
		start_tile_unifs,
		start_tile_geometry,
		true,
		true,
	};

	auto end_tile = path.waypoints.back();
	std::array<float, 8> end_tile_data{
		-1.0f + end_tile.ne * tile_offset_width,
		1.0f - end_tile.se * tile_offset_height,
		-1.0f + end_tile.ne * tile_offset_width,
		1.0f - (end_tile.se + 1) * tile_offset_height,
		-1.0f + (end_tile.ne + 1) * tile_offset_width,
		1.0f - end_tile.se * tile_offset_height,
		-1.0f + (end_tile.ne + 1) * tile_offset_width,
		1.0f - (end_tile.se + 1) * tile_offset_height,
	};

	renderer::resources::VertexInputInfo end_info{
		{renderer::resources::vertex_input_t::V2F32},
		renderer::resources::vertex_layout_t::AOS,
		renderer::resources::vertex_primitive_t::TRIANGLE_STRIP};

	auto const end_data_size = end_tile_data.size() * sizeof(float);
	std::vector<uint8_t> end_verts(end_data_size);
	std::memcpy(end_verts.data(), end_tile_data.data(), end_data_size);

	auto end_tile_mesh = renderer::resources::MeshData(std::move(end_verts), end_info);
	auto end_tile_geometry = renderer->add_mesh_geometry(end_tile_mesh);
	auto end_tile_unifs = obj_shader->new_uniform_input(
		"color",
		Eigen::Vector4f{1.0, 0.5, 0.0, 1.0},
		"model",
		id_matrix,
		"view",
		id_matrix,
		"proj",
		id_matrix);

	auto end_tile_obj = renderer::Renderable{
		end_tile_unifs,
		end_tile_geometry,
		true,
		true,
	};

	this->waypoint_pass->add_renderables({start_tile_obj, end_tile_obj});
}

} // namespace openage::path::tests
