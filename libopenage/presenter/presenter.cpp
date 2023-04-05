// Copyright 2019-2023 the openage authors. See copying.md for legal info.

#include "presenter.h"

#include <eigen3/Eigen/Dense>
#include <iostream>
#include <string>
#include <vector>

#include "engine/engine.h"
#include "event/simulation.h"
#include "log/log.h"
#include "renderer/camera/camera.h"
#include "renderer/gui/gui.h"
#include "renderer/gui/integration/public/gui_application_with_logger.h"
#include "renderer/gui/qml_info.h"
#include "renderer/render_factory.h"
#include "renderer/resources/assets/asset_manager.h"
#include "renderer/resources/shader_source.h"
#include "renderer/resources/texture_info.h"
#include "renderer/stages/screen/screen_renderer.h"
#include "renderer/stages/skybox/skybox_renderer.h"
#include "renderer/stages/terrain/terrain_renderer.h"
#include "renderer/stages/world/world_renderer.h"
#include "renderer/window.h"
#include "util/path.h"

namespace openage::presenter {

Presenter::Presenter(const util::Path &root_dir,
                     const std::shared_ptr<engine::Engine> &engine,
                     const std::shared_ptr<event::Simulation> &simulation) :
	root_dir{root_dir},
	render_passes{},
	engine{engine},
	simulation{simulation} {}


void Presenter::run() {
	log::log(INFO << "presenter launching...");

	this->init_graphics();

	if (this->engine) {
		auto render_factory = std::make_shared<renderer::RenderFactory>(this->terrain_renderer,
		                                                                this->world_renderer);
		this->engine->attach_renderer(render_factory);
	}

	while (not this->window->should_close()) {
		this->gui_app->process_events();
		// TODO: pass button presses and events from GUI to controller

		this->render();

		this->renderer->check_error();

		this->window->update();
	}
	log::log(MSG(info) << "draw loop exited");

	if (this->engine) {
		this->engine->stop();
	}

	if (this->simulation) {
		this->simulation->stop();
	}

	this->window->close();
}

void Presenter::set_engine(const std::shared_ptr<engine::Engine> &engine) {
	this->engine = engine;
	auto render_factory = std::make_shared<renderer::RenderFactory>(this->terrain_renderer,
	                                                                this->world_renderer);
	this->engine->attach_renderer(render_factory);
}

void Presenter::set_simulation(const std::shared_ptr<event::Simulation> &simulation) {
	this->simulation = simulation;
}

std::shared_ptr<qtgui::GuiApplication> Presenter::init_window_system() {
	return std::make_shared<renderer::gui::GuiApplicationWithLogger>();
}

void Presenter::init_graphics() {
	log::log(INFO << "initializing graphics...");

	this->gui_app = this->init_window_system();
	this->window = renderer::Window::create("openage presenter test", 800, 600);
	this->renderer = this->window->make_renderer();

	this->camera = std::make_shared<renderer::camera::Camera>(this->window->get_size());
	this->window->add_resize_callback([this](size_t w, size_t h) {
		this->camera->resize(w, h);
	});

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
		this->asset_manager);
	this->render_passes.push_back(this->terrain_renderer->get_render_pass());

	// Units/buildings
	this->world_renderer = std::make_shared<renderer::world::WorldRenderer>(
		this->window,
		this->renderer,
		this->camera,
		this->root_dir["assets"]["shaders"],
		this->asset_manager,
		this->simulation->get_clock());
	this->render_passes.push_back(this->world_renderer->get_render_pass());

	this->init_gui();
	this->init_final_render_pass();
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
	this->terrain_renderer->update();
	this->world_renderer->update();
	this->gui->render();

	for (auto pass : this->render_passes) {
		this->renderer->render(pass);
	}
}

} // namespace openage::presenter
