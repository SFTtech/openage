// Copyright 2019-2020 the openage authors. See copying.md for legal info.

#include "presenter.h"

#include <eigen3/Eigen/Dense>
#include <iostream>
#include <string>

#include "log/log.h"
#include "renderer/gui/gui.h"
#include "renderer/gui/integration/public/gui_application_with_logger.h"
#include "renderer/gui/qml_info.h"
#include "renderer/resources/shader_source.h"
#include "renderer/resources/texture_info.h"
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

	// dynamically build fragment shader
	// the fragment shader gets as many samplers as we have render passes
	std::string sampler_uniforms{""};
	for (size_t i = 0; i < this->render_passes.size(); ++i) {
		// example: uniform sampler2D tex0;
		sampler_uniforms.append("uniform sampler2D tex" + std::to_string(i) + ";\n");
	}

	// later we access the samplers by index using a switch statement
	std::string switch_cases{""};
	for (size_t i = 0; i < this->render_passes.size(); ++i) {
		// example:
		// case 0:
		//     top_col = texture(tex0, v_uv);
		//     break;
		switch_cases.append(
			"case " + std::to_string(i) + ":\n");
		switch_cases.append(
			"top_col = texture(tex" + std::to_string(i) + ", tex_pos);\n");
		switch_cases.append(
			"break;\n");
	}

	// TODO: Maybe an Array Texture is better since it doesn't require
	// the dynamic shader building and switch-case logic that we do here
	auto frag_shader_src = renderer::resources::ShaderSource(
		renderer::resources::shader_lang_t::glsl,
		renderer::resources::shader_stage_t::fragment,
		R"s(
#version 330

in vec2 tex_pos;
out vec4 out_col;

vec4 top_col;
vec4 bottom_col;
)s"
			+ sampler_uniforms
			+
			R"s(
uniform int tex_count;

void main() {
	// out_col = vec4(0.0 , 1.0 , 0.0, 1.0);
	for(int i = 0; i < tex_count; ++i) {
		bottom_col = out_col;
		switch (i) {
)s"
			+ switch_cases
			+
			R"s(
			default:
				discard;
		}
		out_col = mix(bottom_col, top_col, top_col.a);
	}
}
)s");

	auto quad = renderer->add_mesh_geometry(renderer::resources::MeshData::make_quad());
	auto display_shader = renderer->add_shader({vert_shader_src, frag_shader_src});

	this->texture_unifs = display_shader->create_empty_input();
	for (size_t i = 0; i < this->render_passes.size(); ++i) {
		auto pass = this->render_passes[i];
		auto textures = pass->get_target()->get_texture_targets();

		// TODO: Dirty hack that only selects color textures
		// use this->pass_outputs in the future to assign output
		// textures we want to render
		for (auto tex : textures) {
			auto format = tex->get_info().get_format();
			if (format == renderer::resources::pixel_format::rgba8) {
				auto unif_name = "tex" + std::to_string(i);
				this->texture_unifs->update(unif_name.c_str(), tex);
				break;
			}
		}
	}
	this->texture_unifs->update("tex_count", (int)this->render_passes.size());

	// Update texture unifs if the textures are resized
	this->window->add_resize_callback([this](size_t w, size_t h) {
		this->update_render_pass_unifs();
	});

	renderer::Renderable display_obj{
		this->texture_unifs,
		quad,
		true,
		true,
	};

	auto final_pass = renderer->add_render_pass({display_obj}, renderer->get_display_target());
	this->render_passes.push_back(final_pass);
}

void Presenter::render() {
	for (auto pass : this->render_passes) {
		this->renderer->render(pass);
	}
}

void Presenter::update_render_pass_unifs() {
	// check for size - 1 to exclude the final render pass which is
	// the last entry in the vector
	for (size_t i = 0; i < this->render_passes.size() - 1; ++i) {
		auto pass = this->render_passes[i];
		auto textures = pass->get_target()->get_texture_targets();

		// TODO: Dirty hack that only selects color textures
		// use this->pass_outputs in the future to assign output
		// textures we want to render
		for (auto tex : textures) {
			auto format = tex->get_info().get_format();
			if (format == renderer::resources::pixel_format::rgba8) {
				auto unif_name = "tex" + std::to_string(i);
				this->texture_unifs->update(unif_name.c_str(), tex);
				break;
			}
		}
	}
	// also do size - 1 here
	this->texture_unifs->update("tex_count", (int)this->render_passes.size() - 1);
}

} // namespace openage::presenter
