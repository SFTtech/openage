// Copyright 2019-2024 the openage authors. See copying.md for legal info.

#include "presenter.h"

#include <eigen3/Eigen/Dense>
#include <iostream>
#include <string>
#include <vector>

#include "gamestate/simulation.h"
#include "input/controller/camera/binding_context.h"
#include "input/controller/camera/controller.h"
#include "input/controller/game/binding_context.h"
#include "input/controller/game/controller.h"
#include "input/controller/hud/binding_context.h"
#include "input/controller/hud/controller.h"
#include "input/input_context.h"
#include "input/input_manager.h"
#include "log/log.h"
#include "renderer/camera/camera.h"
#include "renderer/gui/gui.h"
#include "renderer/gui/integration/public/gui_application_with_logger.h"
#include "renderer/render_factory.h"
#include "renderer/render_pass.h"
#include "renderer/render_target.h"
#include "renderer/resources/assets/asset_manager.h"
#include "renderer/resources/shader_source.h"
#include "renderer/resources/texture_info.h"
#include "renderer/stages/camera/manager.h"
#include "renderer/stages/hud/render_stage.h"
#include "renderer/stages/screen/render_stage.h"
#include "renderer/stages/skybox/render_stage.h"
#include "renderer/stages/terrain/render_stage.h"
#include "renderer/stages/world/render_stage.h"
#include "time/time_loop.h"
#include "util/path.h"


namespace openage::presenter {

Presenter::Presenter(const util::Path &root_dir,
                     const std::shared_ptr<gamestate::GameSimulation> &simulation,
                     const std::shared_ptr<time::TimeLoop> &time_loop) :
	root_dir{root_dir},
	render_passes{},
	simulation{simulation},
	time_loop{time_loop} {}


void Presenter::run(const renderer::window_settings window_settings) {
	log::log(INFO << "Presenter: Launching subsystems...");

	this->init_graphics(window_settings);

	this->init_input();

	while (not this->window->should_close()) {
		this->gui_app->process_events();
		// TODO: pass button presses and events from GUI to controller

		this->render();

		this->renderer->check_error();

		this->window->update();
	}

	log::log(MSG(info) << "Presenter: Draw loop exited");

	if (this->simulation) {
		this->simulation->stop();
	}

	if (this->time_loop) {
		this->time_loop->stop();
	}

	this->window->close();
}

void Presenter::set_simulation(const std::shared_ptr<gamestate::GameSimulation> &simulation) {
	this->simulation = simulation;
	auto render_factory = std::make_shared<renderer::RenderFactory>(this->terrain_renderer, this->world_renderer);
	this->simulation->attach_renderer(render_factory);
}

void Presenter::set_time_loop(const std::shared_ptr<time::TimeLoop> &time_loop) {
	this->time_loop = time_loop;
}

std::shared_ptr<qtgui::GuiApplication> Presenter::init_window_system() {
	return std::make_shared<renderer::gui::GuiApplicationWithLogger>();
}

void Presenter::init_graphics(const renderer::window_settings &window_settings) {
	log::log(INFO << "Presenter: Initializing graphics subsystems...");

	// Start up rendering framework
	this->gui_app = this->init_window_system();

	// Window and renderer
	this->window = renderer::Window::create("openage presenter test", window_settings);
	this->renderer = this->window->make_renderer();

	// Asset mangement
	this->asset_manager = std::make_shared<renderer::resources::AssetManager>(
		this->renderer,
		this->root_dir / "assets" / "converted");
	auto missing_tex = this->root_dir / "assets" / "test" / "textures" / "test_missing.sprite";
	this->asset_manager->set_placeholder_animation(missing_tex);

	// Camera
	this->camera = std::make_shared<renderer::camera::Camera>(this->renderer, this->window->get_size());
	this->camera->look_at_coord(coord::scene3{10.0, 10.0, 0}); // Center camera on the map
	this->window->add_resize_callback([this](size_t w, size_t h, double /*scale*/) {
		this->camera->resize(w, h);
	});

	// Camera manager
	this->camera_manager = std::make_shared<renderer::camera::CameraManager>(this->camera);
	// TODO: Make boundaries dynamic based on map size.
	this->camera_manager->set_camera_boundaries(
		renderer::camera::CameraBoundaries{
			renderer::camera::X_BOUND_MIN,
			renderer::camera::X_BOUND_MAX,
			renderer::camera::Y_BOUND_MIN,
			renderer::camera::Y_BOUND_MAX,
			renderer::camera::Z_BOUND_MIN,
			renderer::camera::Z_BOUND_MAX});

	// Skybox
	this->skybox_renderer = std::make_shared<renderer::skybox::SkyboxRenderStage>(
		this->window,
		this->renderer,
		this->root_dir["assets"]["shaders"]);
	this->skybox_renderer->set_color(1.0f, 0.5f, 0.0f, 1.0f);
	this->render_passes.push_back(this->skybox_renderer->get_render_pass());

	// Terrain
	this->terrain_renderer = std::make_shared<renderer::terrain::TerrainRenderStage>(
		this->window,
		this->renderer,
		this->camera,
		this->root_dir["assets"]["shaders"],
		this->asset_manager,
		this->time_loop->get_clock());
	this->render_passes.push_back(this->terrain_renderer->get_render_pass());

	// Units/buildings
	this->world_renderer = std::make_shared<renderer::world::WorldRenderStage>(
		this->window,
		this->renderer,
		this->camera,
		this->root_dir["assets"]["shaders"],
		this->asset_manager,
		this->time_loop->get_clock());
	this->render_passes.push_back(this->world_renderer->get_render_pass());

	// HUD
	this->hud_renderer = std::make_shared<renderer::hud::HudRenderStage>(
		this->window,
		this->renderer,
		this->camera,
		this->root_dir["assets"]["shaders"],
		this->asset_manager,
		this->time_loop->get_clock());
	this->render_passes.push_back(this->hud_renderer->get_render_pass());

	this->init_gui();
	this->init_final_render_pass();

	if (this->simulation) {
		auto render_factory = std::make_shared<renderer::RenderFactory>(this->terrain_renderer, this->world_renderer);
		this->simulation->attach_renderer(render_factory);
	}

	log::log(INFO << "Presenter: Graphics subsystems initialized");
}

void Presenter::init_gui() {
	log::log(INFO << "Presenter: Initializing GUI with Qt backend");

	//// -- gui initialization
	// TODO: Do not use test GUI
	util::Path qml_root = this->root_dir / "assets" / "test" / "qml";
	log::log(INFO << "Presenter: Setting QML root to " << qml_root.resolve_native_path());
	if (not qml_root.is_dir()) {
		throw Error{ERR << "could not find qml root folder " << qml_root};
	}

	util::Path qml_assets = this->root_dir / "assets";
	log::log(INFO << "Presenter: Setting QML asset path to " << qml_assets.resolve_native_path());
	if (not qml_assets.is_dir()) {
		throw Error{ERR << "could not find asset root folder " << qml_assets};
	}

	util::Path qml_root_file = qml_root / "main.qml";
	log::log(INFO << "Presenter: Setting QML root file to " << qml_root_file.resolve_native_path());
	if (not qml_root_file.is_file()) {
		throw Error{ERR << "could not find main.qml file " << qml_root_file};
	}

	// TODO: in order to support qml-mods, the fslike and filelike
	//       library has to be integrated into qt. For now,
	//       figure out the absolute paths here and pass them in.

	this->gui = std::make_shared<renderer::gui::GUI>(
		this->gui_app, // Qt application wrapper
		this->window,  // window for the gui
		qml_root_file, // entry qml file, absolute path.
		qml_root,      // directory to watch for qml file changes
		qml_assets,    // qml data: Engine *, the data directory, ...
		this->renderer // openage renderer
	);

	auto gui_pass = this->gui->get_render_pass();
	this->render_passes.push_back(gui_pass);
}

void Presenter::init_input() {
	log::log(INFO << "Presenter: Initializing input subsystem...");

	this->input_manager = std::make_shared<input::InputManager>();

	this->window->add_key_callback([&](const QKeyEvent &ev) {
		this->input_manager->process(ev);
	});
	this->window->add_mouse_button_callback([&](const QMouseEvent &ev) {
		this->input_manager->process(ev);
	});
	this->window->add_mouse_move_callback([&](const QMouseEvent &ev) {
		this->input_manager->set_mouse(ev.position().x(), ev.position().y());
		this->input_manager->process(ev);
	});
	this->window->add_mouse_wheel_callback([&](const QWheelEvent &ev) {
		this->input_manager->process(ev);
	});

	auto input_ctx = this->input_manager->get_global_context();
	input::setup_defaults(input_ctx);

	// setup simulation controls
	if (this->simulation) {
		log::log(INFO << "Loading game simulation controls");

		// TODO: Remove hardcoding
		auto game_controller = std::make_shared<input::game::Controller>(
			std::unordered_set<size_t>{0, 1, 2, 3}, 0);
		auto engine_context = std::make_shared<input::game::BindingContext>();
		input::game::setup_defaults(engine_context, this->time_loop, this->simulation, this->camera);
		this->input_manager->set_game_controller(game_controller);
		input_ctx->set_game_bindings(engine_context);
	}

	// attach GUI if it's initialized
	if (this->gui) {
		log::log(INFO << "Loading GUI controls");
		this->input_manager->set_gui(this->gui->get_input_handler());
	}

	// setup camera controls
	if (this->camera) {
		log::log(INFO << "Loading camera controls");
		auto camera_controller = std::make_shared<input::camera::Controller>();
		auto camera_context = std::make_shared<input::camera::BindingContext>();
		input::camera::setup_defaults(camera_context, this->camera, this->camera_manager);
		this->input_manager->set_camera_controller(camera_controller);
		input_ctx->set_camera_bindings(camera_context);
	}

	// setup HUD controls
	if (this->hud_renderer) {
		log::log(INFO << "Loading HUD controls");
		auto hud_controller = std::make_shared<input::hud::Controller>();
		auto hud_context = std::make_shared<input::hud::BindingContext>();
		input::hud::setup_defaults(hud_context, this->hud_renderer);
		this->input_manager->set_hud_controller(hud_controller);
		input_ctx->set_hud_bindings(hud_context);
	}

	log::log(INFO << "Presenter: Input subsystem initialized");
}

void Presenter::init_final_render_pass() {
	// Final output to window
	this->screen_renderer = std::make_shared<renderer::screen::ScreenRenderStage>(
		this->window,
		this->renderer,
		this->root_dir["assets"]["shaders"]);
	std::vector<std::shared_ptr<renderer::RenderTarget>> targets{};
	for (auto pass : this->render_passes) {
		targets.push_back(pass->get_target());
	}
	this->screen_renderer->set_render_targets(targets);
	this->render_passes.push_back(this->screen_renderer->get_render_pass());

	// Update final render pass if the textures are reassigned on resize
	// TODO: This REQUIRES that all other render passes have already been
	//       resized
	this->window->add_resize_callback([this](size_t, size_t, double /*scale*/) {
		// Acquire the render targets for all previous passes
		std::vector<std::shared_ptr<renderer::RenderTarget>> targets{};
		for (size_t i = 0; i < this->render_passes.size() - 1; ++i) {
			targets.push_back(this->render_passes[i]->get_target());
		}
		this->screen_renderer->set_render_targets(targets);
	});
}

void Presenter::render() {
	// TODO: Pass current time to update() instead of fetching it in renderer
	this->camera_manager->update();
	this->terrain_renderer->update();
	this->world_renderer->update();
	this->hud_renderer->update();
	this->gui->render();

	for (auto &pass : this->render_passes) {
		this->renderer->render(pass);
	}
}

} // namespace openage::presenter
