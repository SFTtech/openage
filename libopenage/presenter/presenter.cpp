// Copyright 2019-2023 the openage authors. See copying.md for legal info.

#include "presenter.h"

#include <eigen3/Eigen/Dense>
#include <iostream>
#include <string>
#include <vector>

#include "event/time_loop.h"
#include "gamestate/simulation.h"
#include "input/controller/camera/binding_context.h"
#include "input/controller/camera/controller.h"
#include "input/controller/game/binding_context.h"
#include "input/controller/game/controller.h"
#include "input/input_context.h"
#include "input/input_manager.h"
#include "log/log.h"
#include "renderer/camera/camera.h"
#include "renderer/gui/gui.h"
#include "renderer/gui/integration/public/gui_application_with_logger.h"
#include "renderer/gui/qml_info.h"
#include "renderer/render_factory.h"
#include "renderer/resources/assets/asset_manager.h"
#include "renderer/resources/shader_source.h"
#include "renderer/resources/texture_info.h"
#include "renderer/stages/camera/manager.h"
#include "renderer/stages/screen/screen_renderer.h"
#include "renderer/stages/skybox/skybox_renderer.h"
#include "renderer/stages/terrain/terrain_renderer.h"
#include "renderer/stages/world/world_renderer.h"
#include "renderer/window.h"
#include "util/path.h"

namespace openage::presenter {

Presenter::Presenter(const util::Path &root_dir,
                     const std::shared_ptr<gamestate::GameSimulation> &simulation,
                     const std::shared_ptr<event::TimeLoop> &time_loop) :
	root_dir{root_dir},
	render_passes{},
	simulation{simulation},
	time_loop{time_loop} {}


void Presenter::run() {
	log::log(INFO << "presenter launching...");

	this->init_graphics();

	this->init_input();

	while (not this->window->should_close()) {
		this->gui_app->process_events();
		// TODO: pass button presses and events from GUI to controller

		this->render();

		this->renderer->check_error();

		this->window->update();
	}
	log::log(MSG(info) << "Draw loop exited");

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
	auto render_factory = std::make_shared<renderer::RenderFactory>(this->terrain_renderer,
	                                                                this->world_renderer);
	this->simulation->attach_renderer(render_factory);
}

void Presenter::set_time_loop(const std::shared_ptr<event::TimeLoop> &time_loop) {
	this->time_loop = time_loop;
}

std::shared_ptr<qtgui::GuiApplication> Presenter::init_window_system() {
	return std::make_shared<renderer::gui::GuiApplicationWithLogger>();
}

void Presenter::init_graphics() {
	log::log(INFO << "initializing graphics...");

	this->gui_app = this->init_window_system();
	this->window = renderer::Window::create("openage presenter test", 800, 600);
	this->renderer = this->window->make_renderer();

	// Camera
	this->camera = std::make_shared<renderer::camera::Camera>(this->renderer,
	                                                          this->window->get_size());
	this->window->add_resize_callback([this](size_t w, size_t h) {
		this->camera->resize(w, h);
	});

	this->camera_manager = std::make_shared<renderer::camera::CameraManager>(this->camera);

	// Asset mangement
	this->asset_manager = std::make_shared<renderer::resources::AssetManager>(this->renderer);

	// Skybox
	this->skybox_renderer = std::make_shared<renderer::skybox::SkyboxRenderer>(
		this->window,
		this->renderer,
		this->root_dir["assets"]["shaders"]);
	this->skybox_renderer->set_color(1.0f, 0.5f, 0.0f, 1.0f);
	this->render_passes.push_back(this->skybox_renderer->get_render_pass());

	// Terrain
	this->terrain_renderer = std::make_shared<renderer::terrain::TerrainRenderer>(
		this->window,
		this->renderer,
		this->camera,
		this->root_dir["assets"]["shaders"],
		this->asset_manager,
		this->time_loop->get_clock());
	this->render_passes.push_back(this->terrain_renderer->get_render_pass());

	// Units/buildings
	this->world_renderer = std::make_shared<renderer::world::WorldRenderer>(
		this->window,
		this->renderer,
		this->camera,
		this->root_dir["assets"]["shaders"],
		this->asset_manager,
		this->time_loop->get_clock());
	this->render_passes.push_back(this->world_renderer->get_render_pass());

	this->init_gui();
	this->init_final_render_pass();

	if (this->simulation) {
		auto render_factory = std::make_shared<renderer::RenderFactory>(this->terrain_renderer,
		                                                                this->world_renderer);
		this->simulation->attach_renderer(render_factory);
	}
}

void Presenter::init_gui() {
	//// -- gui initialization
	// TODO: Do not use test GUI
	util::Path qml_root = this->root_dir / "assets" / "test" / "qml";
	if (not qml_root.is_dir()) {
		throw Error{ERR << "could not find qml root folder " << qml_root};
	}

	util::Path qml_assets = this->root_dir / "assets";
	if (not qml_assets.is_dir()) {
		throw Error{ERR << "could not find asset root folder " << qml_assets};
	}

	util::Path qml_root_file = qml_root / "main.qml";
	if (not qml_root_file.is_file()) {
		throw Error{ERR << "could not find main.qml file " << qml_root_file};
	}

	// TODO: in order to support qml-mods, the fslike and filelike
	//       library has to be integrated into qt. For now,
	//       figure out the absolute paths here and pass them in.

	this->gui = std::make_shared<renderer::gui::GUI>(
		this->gui_app, // Qt application wrapper
		this->window, // window for the gui
		qml_root_file, // entry qml file, absolute path.
		qml_root, // directory to watch for qml file changes
		qml_assets, // qml data: Engine *, the data directory, ...
		this->renderer // openage renderer
	);

	auto gui_pass = this->gui->get_render_pass();
	this->render_passes.push_back(gui_pass);
}

void Presenter::init_input() {
	log::log(INFO << "initializing inputs...");

	this->input_manager = std::make_shared<input::InputManager>();

	this->window->add_key_callback([&](const QKeyEvent &ev) {
		this->input_manager->process(ev);
	});
	this->window->add_mouse_button_callback([&](const QMouseEvent &ev) {
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
		// TODO: Remove hardcoding
		auto engine_controller = std::make_shared<input::game::Controller>(
			std::unordered_set<size_t>{0, 1, 2, 3}, 0);
		auto engine_context = std::make_shared<input::game::BindingContext>();
		input::game::setup_defaults(engine_context, this->time_loop, this->simulation, this->camera);
		this->input_manager->set_engine_controller(engine_controller);
		input_ctx->set_engine_bindings(engine_context);
	}

	// attach GUI if it's initialized
	if (this->gui) {
		this->input_manager->set_gui(this->gui->get_input_handler());
	}

	// setup camera controls
	if (this->camera) {
		auto camera_controller = std::make_shared<input::camera::Controller>();
		auto camera_context = std::make_shared<input::camera::BindingContext>();
		input::camera::setup_defaults(camera_context, this->camera, this->camera_manager);
		this->input_manager->set_camera_controller(camera_controller);
		input_ctx->set_camera_bindings(camera_context);
	}
}

void Presenter::init_final_render_pass() {
	// Final output to window
	this->screen_renderer = std::make_shared<renderer::screen::ScreenRenderer>(
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
	this->window->add_resize_callback([this](size_t, size_t) {
		// Acquire the render targets for all previous passes
		std::vector<std::shared_ptr<renderer::RenderTarget>> targets{};
		for (size_t i = 0; i < this->render_passes.size() - 1; ++i) {
			targets.push_back(this->render_passes[i]->get_target());
		}
		this->screen_renderer->set_render_targets(targets);
	});
}

void Presenter::render() {
	this->camera_manager->update();
	this->terrain_renderer->update();
	this->world_renderer->update();
	this->gui->render();

	for (auto pass : this->render_passes) {
		this->renderer->render(pass);
	}
}

} // namespace openage::presenter
