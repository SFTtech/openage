// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#include "stresstest_0.h"

#include <eigen3/Eigen/Dense>

#include "coord/tile.h"
#include "renderer/camera/camera.h"
#include "renderer/gui/integration/public/gui_application_with_logger.h"
#include "renderer/opengl/window.h"
#include "renderer/render_factory.h"
#include "renderer/render_pass.h"
#include "renderer/render_target.h"
#include "renderer/resources/assets/asset_manager.h"
#include "renderer/resources/shader_source.h"
#include "renderer/stages/camera/manager.h"
#include "renderer/stages/screen/render_stage.h"
#include "renderer/stages/skybox/render_stage.h"
#include "renderer/stages/terrain/render_entity.h"
#include "renderer/stages/terrain/render_stage.h"
#include "renderer/stages/world/render_entity.h"
#include "renderer/stages/world/render_stage.h"
#include "renderer/uniform_buffer.h"
#include "time/clock.h"
#include "util/fps.h"


namespace openage::renderer::tests {

void renderer_stresstest_0(const util::Path &path) {
	auto qtapp = std::make_shared<gui::GuiApplicationWithLogger>();

	window_settings settings;
	settings.width = 1024;
	settings.height = 768;
	settings.vsync = false;
	settings.debug = true;
	auto window = std::make_shared<opengl::GlWindow>("openage renderer test", settings);
	auto renderer = window->make_renderer();

	// Clock required by world renderer for timing animation frames
	auto clock = std::make_shared<time::Clock>();

	// Camera
	// our viewport into the game world
	auto camera = std::make_shared<renderer::camera::Camera>(renderer,
	                                                         window->get_size(),
	                                                         Eigen::Vector3f{17.0f, 10.0f, 7.0f});
	auto cam_unifs = camera->get_uniform_buffer()->create_empty_input();
	cam_unifs->update(
		"view",
		camera->get_view_matrix(),
		"proj",
		camera->get_projection_matrix(),
		"inv_zoom",
		1.0f / camera->get_zoom());
	auto viewport_size = camera->get_viewport_size();
	Eigen::Vector2f viewport_size_vec{
		1.0f / static_cast<float>(viewport_size[0]),
		1.0f / static_cast<float>(viewport_size[1])};
	cam_unifs->update("inv_viewport_size", viewport_size_vec);
	camera->get_uniform_buffer()->update_uniforms(cam_unifs);

	// Render stages
	// every stage use a different subrenderer that manages renderables,
	// shaders, textures & more.
	std::vector<std::shared_ptr<RenderPass>>
		render_passes{};

	// TODO: Make this optional for subrenderers?
	auto asset_manager = std::make_shared<renderer::resources::AssetManager>(
		renderer,
		path["assets"]["test"]);

	// Renders the background
	auto skybox_renderer = std::make_shared<renderer::skybox::SkyboxRenderStage>(
		window,
		renderer,
		path["assets"]["shaders"]);
	skybox_renderer->set_color(1.0f, 0.5f, 0.0f, 1.0f); // orange color

	// Renders the terrain in 3D
	auto terrain_renderer = std::make_shared<renderer::terrain::TerrainRenderStage>(
		window,
		renderer,
		camera,
		path["assets"]["shaders"],
		asset_manager,
		clock);

	// Renders units/buildings/other objects
	auto world_renderer = std::make_shared<renderer::world::WorldRenderStage>(
		window,
		renderer,
		camera,
		path["assets"]["shaders"],
		asset_manager,
		clock);

	// Store the render passes of the renderers
	// The order is important as its also the order in which they
	// are rendered and drawn onto the screen.
	render_passes.push_back(skybox_renderer->get_render_pass());
	render_passes.push_back(terrain_renderer->get_render_pass());
	render_passes.push_back(world_renderer->get_render_pass());

	// Final output on screen has its own subrenderer
	// It takes the outputs of all previous render passes
	// and blends them together
	auto screen_renderer = std::make_shared<renderer::screen::ScreenRenderStage>(
		window,
		renderer,
		path["assets"]["shaders"]);
	std::vector<std::shared_ptr<renderer::RenderTarget>> targets{};
	for (auto &pass : render_passes) {
		targets.push_back(pass->get_target());
	}
	screen_renderer->set_render_targets(targets);

	window->add_resize_callback([&](size_t, size_t, double /*scale*/) {
		// Acquire the render targets for all previous passes
		std::vector<std::shared_ptr<renderer::RenderTarget>> targets{};
		for (size_t i = 0; i < render_passes.size() - 1; ++i) {
			targets.push_back(render_passes[i]->get_target());
		}
		screen_renderer->set_render_targets(targets);
	});

	render_passes.push_back(screen_renderer->get_render_pass());

	// Create some entities to populate the scene
	auto render_factory = std::make_shared<RenderFactory>(terrain_renderer, world_renderer);

	// Fill a 10x10 terrain grid with height values
	auto terrain_size = util::Vector2s{10, 10};
	std::vector<std::pair<terrain::RenderEntity::terrain_elevation_t, std::string>> tiles{};
	tiles.reserve(terrain_size[0] * terrain_size[1]);
	for (size_t i = 0; i < terrain_size[0] * terrain_size[1]; ++i) {
		tiles.emplace_back(0.0f, "./textures/test_terrain.terrain");
	}

	// Create entity for terrain rendering
	auto terrain0 = render_factory->add_terrain_render_entity(terrain_size,
	                                                          coord::tile_delta{0, 0});

	// send the terrain data to the terrain renderer
	terrain0->update(terrain_size, tiles);

	// World entities
	std::vector<std::shared_ptr<renderer::world::RenderEntity>> render_entities{};
	auto add_world_entity = [&](const coord::phys3 initial_pos,
	                            const time::time_t time) {
		const auto animation_path = "./textures/test_tank_mirrored.sprite";

		auto position = curve::Continuous<coord::phys3>{nullptr, 0, "", nullptr, coord::phys3(0, 0, 0)};
		position.set_insert(time, initial_pos);
		position.set_insert(time + 1, initial_pos + coord::phys3_delta{0, 3, 0});
		position.set_insert(time + 2, initial_pos + coord::phys3_delta{3, 6, 0});
		position.set_insert(time + 3, initial_pos + coord::phys3_delta{6, 6, 0});
		position.set_insert(time + 4, initial_pos + coord::phys3_delta{9, 3, 0});
		position.set_insert(time + 5, initial_pos + coord::phys3_delta{9, 0, 0});
		position.set_insert(time + 6, initial_pos + coord::phys3_delta{6, -3, 0});
		position.set_insert(time + 7, initial_pos + coord::phys3_delta{3, -3, 0});
		position.set_insert(time + 8, initial_pos);

		auto angle = curve::Segmented<coord::phys_angle_t>{nullptr, 0};
		angle.set_insert(time, coord::phys_angle_t::from_int(315));
		angle.set_insert_jump(time + 1, coord::phys_angle_t::from_int(315), coord::phys_angle_t::from_int(270));
		angle.set_insert_jump(time + 2, coord::phys_angle_t::from_int(270), coord::phys_angle_t::from_int(225));
		angle.set_insert_jump(time + 3, coord::phys_angle_t::from_int(225), coord::phys_angle_t::from_int(180));
		angle.set_insert_jump(time + 4, coord::phys_angle_t::from_int(180), coord::phys_angle_t::from_int(135));
		angle.set_insert_jump(time + 5, coord::phys_angle_t::from_int(135), coord::phys_angle_t::from_int(90));
		angle.set_insert_jump(time + 6, coord::phys_angle_t::from_int(90), coord::phys_angle_t::from_int(45));
		angle.set_insert_jump(time + 7, coord::phys_angle_t::from_int(45), coord::phys_angle_t::from_int(0));
		angle.set_insert_jump(time + 8, coord::phys_angle_t::from_int(0), coord::phys_angle_t::from_int(315));

		auto entity = render_factory->add_world_render_entity();
		entity->update(render_entities.size(),
		               position,
		               angle,
		               animation_path,
		               time);
		render_entities.push_back(entity);
	};

	// Stop after 500 entities
	size_t entity_limit = 500;

	clock->start();

	util::FrameCounter timer;

	add_world_entity(coord::phys3(0.0f, 3.0f, 0.0f), clock->get_time());
	time::time_t next_entity = clock->get_real_time() + 0.1;
	while (render_entities.size() <= entity_limit) {
		// Print FPS
		timer.frame();
		std::cout
			<< "Entities: " << render_entities.size()
			<< " -- "
			<< "FPS: " << timer.fps << "\r" << std::flush;

		qtapp->process_events();

		// Advance time
		clock->update_time();
		auto current_time = clock->get_real_time();
		if (current_time > next_entity) {
			add_world_entity(coord::phys3(0.0f, 3.0f, 0.0f), clock->get_time());
			next_entity = current_time + 0.1;
		}

		// Update the renderables of the subrenderers
		terrain_renderer->update();
		world_renderer->update();

		// Draw everything
		for (auto &pass : render_passes) {
			renderer->render(pass);
		}

		renderer->check_error();

		// Display final output on screen
		window->update();
	}

	clock->stop();
	log::log(MSG(info) << "Stopped after rendering " << render_entities.size() << " entities");

	window->close();
}

} // namespace openage::renderer::tests
