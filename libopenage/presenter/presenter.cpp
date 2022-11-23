// Copyright 2019-2020 the openage authors. See copying.md for legal info.

#include "presenter.h"

#include <eigen3/Eigen/Dense>
#include <iostream>
#include <string>
#include <vector>

#include "log/log.h"
#include "renderer/gui/gui.h"
#include "renderer/gui/integration/public/gui_application_with_logger.h"
#include "renderer/gui/qml_info.h"
#include "renderer/resources/shader_source.h"
#include "renderer/resources/texture_info.h"
#include "renderer/stages/screen/screen_renderer.h"
#include "renderer/stages/skybox/skybox_renderer.h"
#include "renderer/stages/terrain/terrain_renderer.h"
#include "renderer/window.h"
#include "util/path.h"

namespace openage::presenter {

Presenter::Presenter(const util::Path &root_dir) :
	root_dir{root_dir},
	render_passes{} {}


void Presenter::run() {
	log::log(INFO << "presenter launching...");

	this->init_graphics();

	while (not this->window->should_close()) {
		this->gui_app->process_events();
		// this->gui->process_events();

		this->gui->render();
		this->render();

		this->renderer->check_error();

		this->window->update();
	}
	log::log(MSG(info) << "draw loop exited");

	this->window->close();
}

std::shared_ptr<qtgui::GuiApplication> Presenter::init_window_system() {
	return std::make_shared<renderer::gui::GuiApplicationWithLogger>();
}

void Presenter::init_graphics() {
	log::log(INFO << "initializing graphics...");

	this->gui_app = this->init_window_system();
	this->window = renderer::Window::create("openage presenter test", 800, 600);
	this->renderer = this->window->make_renderer();

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
		this->root_dir["assets"]["shaders"]);
	this->render_passes.push_back(this->terrain_renderer->get_render_pass());

	this->init_gui();
	this->init_final_render_pass();
}

void Presenter::init_gui() {
	//// -- gui initialization
	util::Path qml_root = this->root_dir / "assets" / "qml";
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
		// remove the final pass
		this->render_passes.pop_back();

		// Acquire the render targets for all previous passes
		std::vector<std::shared_ptr<renderer::RenderTarget>> targets{};
		for (auto pass : this->render_passes) {
			targets.push_back(pass->get_target());
		}
		// this will recreate this renderer's pass
		// so we have to add it to the list of passes again
		this->screen_renderer->set_render_targets(targets);
		this->render_passes.push_back(this->screen_renderer->get_render_pass());
	});
}

void Presenter::render() {
	for (auto pass : this->render_passes) {
		this->renderer->render(pass);
	}
}

} // namespace openage::presenter
