// demo_shader_commands.h
#pragma once

#include "util/path.h"

#include <eigen3/Eigen/Dense>
#include <QKeyEvent>

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

namespace openage::renderer::tests {

void renderer_demo_7(const util::Path &path) {
	// Basic setup
	auto qtapp = std::make_shared<gui::GuiApplicationWithLogger>();
	window_settings settings;
	settings.width = 800;
	settings.height = 600;
	settings.debug = true;

	auto window = std::make_shared<opengl::GlWindow>("Shader Commands Demo", settings);
	auto renderer = window->make_renderer();
	auto camera = std::make_shared<renderer::camera::Camera>(renderer, window->get_size());
	auto clock = std::make_shared<time::Clock>();
	auto asset_manager = std::make_shared<renderer::resources::AssetManager>(
		renderer,
		path["assets"]["test"]);
	auto cam_manager = std::make_shared<renderer::camera::CameraManager>(camera);

	auto shaderdir = path / "assets" / "test" / "shaders";

	std::vector<std::shared_ptr<RenderPass>>
		render_passes{};

	// Initialize world renderer with shader commands
	auto world_renderer = std::make_shared<renderer::world::WorldRenderStage>(
		window,
		renderer,
		camera,
		shaderdir,
		shaderdir, // Temporarily, Shader commands config has the same path with shaders for this demo
		asset_manager,
		clock);

	render_passes.push_back(world_renderer->get_render_pass());

	auto screen_renderer = std::make_shared<renderer::screen::ScreenRenderStage>(
		window,
		renderer,
		path["assets"]["shaders"]);
	std::vector<std::shared_ptr<renderer::RenderTarget>> targets{};
	for (auto &pass : render_passes) {
		targets.push_back(pass->get_target());
	}
	screen_renderer->set_render_targets(targets);

	render_passes.push_back(screen_renderer->get_render_pass());

	auto render_factory = std::make_shared<RenderFactory>(nullptr, world_renderer);

	auto entity1 = render_factory->add_world_render_entity();
	entity1->update(0, coord::phys3(0.0f, 0.0f, 0.0f), "./textures/test_gaben.sprite");

	auto entity2 = render_factory->add_world_render_entity();
	entity2->update(1, coord::phys3(3.0f, 0.0f, 0.0f), "./textures/test_gaben.sprite");

	auto entity3 = render_factory->add_world_render_entity();
	entity3->update(2, coord::phys3(-3.0f, 0.0f, 0.0f), "./textures/test_gaben.sprite");

	// Main loop
	while (not window->should_close()) {
		qtapp->process_events();

		// Update camera matrices
		cam_manager->update();

		world_renderer->update();

		for (auto &pass : render_passes) {
			renderer->render(pass);
		}

		renderer->check_error();

		window->update();
	}
}

} // namespace openage::renderer::tests