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

	this->skybox_renderer = std::make_shared<renderer::skybox::SkyboxRenderer>(
		this->window,
		this->renderer,
		this->root_dir["assets"]["shaders"]);
	this->skybox_renderer->set_color(1.0f, 0.5f, 0.0f, 1.0f);
	this->render_passes.push_back(this->skybox_renderer->get_render_pass());

	this->terrain_renderer = std::make_shared<renderer::terrain::TerrainRenderer>(
		this->window,
		this->renderer,
		this->root_dir["assets"]["shaders"]);
	this->render_passes.push_back(this->terrain_renderer->get_render_pass());

	this->init_gui();
	this->init_render_pass();
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

void Presenter::init_render_pass() {
	util::Path shaderdir = this->root_dir["assets"]["shaders"];

	auto vert_shader_file = (shaderdir / "final.vert.glsl").open();
	auto vert_shader_src = renderer::resources::ShaderSource(
		renderer::resources::shader_lang_t::glsl,
		renderer::resources::shader_stage_t::vertex,
		vert_shader_file.read());
	vert_shader_file.close();

	auto frag_shader_file = (shaderdir / "final.frag.glsl").open();
	auto frag_shader_src = renderer::resources::ShaderSource(
		renderer::resources::shader_lang_t::glsl,
		renderer::resources::shader_stage_t::fragment,
		frag_shader_file.read());
	frag_shader_file.close();

	auto quad = renderer->add_mesh_geometry(renderer::resources::MeshData::make_quad());
	this->display_shader = renderer->add_shader({vert_shader_src, frag_shader_src});

	std::vector<renderer::Renderable> output_layers{};
	output_layers.reserve(this->render_passes.size());
	for (size_t i = 0; i < this->render_passes.size(); ++i) {
		auto pass = this->render_passes[i];
		auto textures = pass->get_target()->get_texture_targets();
		auto texture_unif = this->display_shader->create_empty_input();

		// TODO: Dirty hack that only selects color textures
		// use this->pass_outputs in the future to assign output
		// textures we want to render
		for (auto tex : textures) {
			auto format = tex->get_info().get_format();
			if (format == renderer::resources::pixel_format::rgba8) {
				texture_unif->update("tex", tex);
				break;
			}
		}
		renderer::Renderable display_obj{
			texture_unif,
			quad,
			true,
			false,
		};
		output_layers.push_back(display_obj);
	}
	auto final_pass = renderer->add_render_pass(output_layers, renderer->get_display_target());
	this->render_passes.push_back(final_pass);

	// Update render pass if the textures are reassigned on resize
	this->window->add_resize_callback([this](size_t, size_t) {
		this->update_render_pass_unifs();
	});
}

void Presenter::render() {
	for (auto pass : this->render_passes) {
		this->renderer->render(pass);
	}
}

void Presenter::update_render_pass_unifs() {
	// Remove the final pass because we have to replace it
	// TODO: We should update the renderables instead
	this->render_passes.pop_back();

	auto quad = renderer->add_mesh_geometry(renderer::resources::MeshData::make_quad());

	std::vector<renderer::Renderable> output_layers{};
	output_layers.reserve(this->render_passes.size());
	for (size_t i = 0; i < this->render_passes.size(); ++i) {
		auto pass = this->render_passes[i];
		auto textures = pass->get_target()->get_texture_targets();
		auto texture_unif = this->display_shader->create_empty_input();

		// TODO: Dirty hack that only selects color textures
		// use this->pass_outputs in the future to assign output
		// textures we want to render
		for (auto tex : textures) {
			auto format = tex->get_info().get_format();
			if (format == renderer::resources::pixel_format::rgba8) {
				texture_unif->update("tex", tex);
				break;
			}
		}
		renderer::Renderable display_obj{
			texture_unif,
			quad,
			true,
			false,
		};
		output_layers.push_back(display_obj);
	}
	auto final_pass = renderer->add_render_pass(output_layers, renderer->get_display_target());
	this->render_passes.push_back(final_pass);
}

} // namespace openage::presenter
