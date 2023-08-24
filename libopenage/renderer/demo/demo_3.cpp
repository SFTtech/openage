// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#include "demo_3.h"

#include <eigen3/Eigen/Dense>
#include <QKeyEvent>

#include "renderer/camera/camera.h"
#include "renderer/gui/integration/public/gui_application_with_logger.h"
#include "renderer/opengl/window.h"
#include "renderer/render_factory.h"
#include "renderer/resources/assets/asset_manager.h"
#include "renderer/resources/shader_source.h"
#include "renderer/stages/camera/manager.h"
#include "renderer/stages/screen/screen_renderer.h"
#include "renderer/stages/skybox/skybox_renderer.h"
#include "renderer/stages/terrain/terrain_render_entity.h"
#include "renderer/stages/terrain/terrain_renderer.h"
#include "renderer/stages/world/world_render_entity.h"
#include "renderer/stages/world/world_renderer.h"
#include "renderer/uniform_buffer.h"
#include "time/clock.h"

namespace openage::renderer::tests {

void renderer_demo_3(const util::Path &path) {
	auto qtapp = std::make_shared<gui::GuiApplicationWithLogger>();

	auto window = std::make_shared<opengl::GlWindow>("openage renderer test", 800, 600, true);
	auto renderer = window->make_renderer();

	// Clock required by world renderer for timing animation frames
	// (we never advance time in this demo though, so it has no significance)
	auto clock = std::make_shared<time::Clock>();

	// Camera
	// our viewport into the game world
	auto camera = std::make_shared<renderer::camera::Camera>(renderer, window->get_size());
	window->add_resize_callback([&](size_t w, size_t h, double /*scale*/) {
		camera->resize(w, h);
	});

	// The camera manager handles camera movement and zooming
	// it is updated each frame before the render stages
	auto cam_manager = std::make_shared<renderer::camera::CameraManager>(camera);

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
	auto skybox_renderer = std::make_shared<renderer::skybox::SkyboxRenderer>(
		window,
		renderer,
		path["assets"]["shaders"]);
	skybox_renderer->set_color(1.0f, 0.5f, 0.0f, 1.0f); // orange color

	// Renders the terrain in 3D
	auto terrain_renderer = std::make_shared<renderer::terrain::TerrainRenderer>(
		window,
		renderer,
		camera,
		path["assets"]["shaders"],
		asset_manager,
		clock);

	// Renders units/buildings/other objects
	auto world_renderer = std::make_shared<renderer::world::WorldRenderer>(
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
	auto screen_renderer = std::make_shared<renderer::screen::ScreenRenderer>(
		window,
		renderer,
		path["assets"]["shaders"]);
	std::vector<std::shared_ptr<renderer::RenderTarget>> targets{};
	for (auto &pass : render_passes) {
		targets.push_back(pass->get_target());
	}
	screen_renderer->set_render_targets(targets);

	render_passes.push_back(screen_renderer->get_render_pass());

	window->add_resize_callback([&](size_t, size_t, double /*scale*/) {
		std::vector<std::shared_ptr<renderer::RenderTarget>> targets{};
		for (size_t i = 0; i < render_passes.size() - 1; ++i) {
			targets.push_back(render_passes[i]->get_target());
		}
		screen_renderer->set_render_targets(targets);
	});

	// Create some entities to populate the scene
	auto render_factory = std::make_shared<RenderFactory>(terrain_renderer, world_renderer);

	// Terrain
	auto terrain0 = render_factory->add_terrain_render_entity();

	// Fill a 10x10 terrain grid with height values
	auto terrain_size = util::Vector2s{10, 10};
	std::vector<float> height_map{};
	height_map.reserve(terrain_size[0] * terrain_size[1]);
	for (size_t i = 0; i < terrain_size[0] * terrain_size[1]; ++i) {
		height_map.push_back(0.0f);
	}

	// Create "test bumps" in the terrain to check if rendering works
	height_map[11] = 1.0f;
	height_map[23] = 2.3f;
	height_map[42] = 4.2f;
	height_map[69] = 6.9f; // nice

	// A hill
	height_map[55] = 3.0f; // center
	height_map[45] = 2.0f; // bottom left slope
	height_map[35] = 1.0f;
	height_map[56] = 1.0f; // bottom right slope (little steeper)
	height_map[65] = 2.0f; // top right slope
	height_map[75] = 1.0f;
	height_map[54] = 2.0f; // top left slope
	height_map[53] = 1.0f;

	// send the terrain data to the terrain renderer
	terrain0->update(terrain_size,
	                 height_map,
	                 "./textures/test_terrain.terrain");

	// World entities
	auto world0 = render_factory->add_world_render_entity();
	world0->update(0, coord::phys3(3.0f, 3.0f, 0.0f), "./textures/test_gaben.sprite");

	// should behind gaben and caught by depth test
	auto world1 = render_factory->add_world_render_entity();
	world1->update(1, coord::phys3(4.0f, 1.0f, 0.0f), "./textures/test_missing.sprite");

	auto world2 = render_factory->add_world_render_entity();
	world2->update(2, coord::phys3(1.0f, 3.0f, 0.0f), "./textures/test_missing.sprite");

	// Zoom in/out with mouse wheel
	window->add_mouse_wheel_callback([&](const QWheelEvent &ev) {
		auto delta = ev.angleDelta().y() / 120;

		// zoom_frame updates the camera zoom level in the next drawn frame
		if (delta < 0) {
			cam_manager->zoom_frame(renderer::camera::ZoomDirection::OUT, 0.05f);
		}
		else if (delta > 0) {
			cam_manager->zoom_frame(renderer::camera::ZoomDirection::IN, 0.05f);
		}
	});

	// Move around the scene with WASD
	window->add_key_callback([&](const QKeyEvent &ev) {
		if (ev.type() == QEvent::KeyPress) {
			auto key = ev.key();

			// move_frame moves the camera in the specified direction in the next drawn frame
			switch (key) {
			case Qt::Key_W: { // forward
				cam_manager->move_frame(renderer::camera::MoveDirection::FORWARD, 0.5f);
			} break;
			case Qt::Key_A: { // left
				cam_manager->move_frame(renderer::camera::MoveDirection::LEFT, 0.5f);
			} break;
			case Qt::Key_S: { // back
				cam_manager->move_frame(renderer::camera::MoveDirection::BACKWARD, 0.5f);
			} break;
			case Qt::Key_D: { // right
				cam_manager->move_frame(renderer::camera::MoveDirection::RIGHT, 0.5f);
			} break;
			default:
				break;
			}
		}
	});

	log::log(INFO << "Instructions:");
	log::log(INFO << "  1. Move the camera with WASD");
	log::log(INFO << "  2. Zoom in and out with MOUSE WHEEL");

	while (not window->should_close()) {
		qtapp->process_events();

		// update the camera matrices
		cam_manager->update();

		// Update the renderables of the subrenderers
		// camera zoom/position changes are also handled in here
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
	window->close();
}

} // namespace openage::renderer::tests
