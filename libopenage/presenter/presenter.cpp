// Copyright 2019-2020 the openage authors. See copying.md for legal info.

#include "presenter.h"

#include <eigen3/Eigen/Dense>

#include "../log/log.h"
#include "../gui/gui.h"
#include "../util/path.h"


namespace openage::presenter {

Presenter::Presenter(const util::Path &root_dir)
	:
	root_dir{root_dir} {}


void Presenter::run() {
	log::log(INFO << "presenter launching...");

	this->window = renderer::Window::create("openage presenter test", 800, 600);

	this->renderer = this->window->make_renderer();

	this->window->add_resize_callback([&] (size_t w, size_t h) {
		float aspectRatio = float(w) / float(h);
		float x_scale = 1.0 / aspectRatio;

		Eigen::Matrix4f pmat;
		pmat << x_scale, 0, 0, 0,
		              0, 1, 0, 0,
		              0, 0, 1, 0,
		              0, 0, 0, 1;

		// TODO!
		/*
		proj_unif->update("proj", pmat);

		color_texture = renderer->add_texture(resources::Texture2dInfo(w, h, resources::pixel_format::rgba8));
		id_texture = renderer->add_texture(resources::Texture2dInfo(w, h, resources::pixel_format::r32ui));
		depth_texture = renderer->add_texture(resources::Texture2dInfo(w, h, resources::pixel_format::depth24));
		fbo = renderer->create_texture_target( { color_texture, id_texture, depth_texture } );

		color_texture_unif->update("color_texture", color_texture);

		texture_data_valid = false;
		pass1->set_target(fbo);
		*/
	});

	//// -- initialize the gui
	util::Path qml_root = this->root_dir / "assets" / "qml";
	if (not qml_root.is_dir()) {
		throw Error{ERR << "could not find qml root folder " << qml_root};
	}

	util::Path qml_root_file = qml_root / "main.qml";
	if (not qml_root_file.is_file()) {
		throw Error{ERR << "could not find main.qml file " << qml_root_file};
	}

	// TODO: in order to support qml-mods, the fslike and filelike
	//       library has to be integrated into qt. For now,
	//       figure out the absolute paths here and pass them in.

	std::string qml_root_str = qml_root.resolve_native_path();
	std::string qml_root_file_str = qml_root_file.resolve_native_path();

	/*
	this->gui = std::make_unique<gui::GUI>(
		this->window,                // sdl window for the gui
		qml_root_file_str,           // entry qml file, absolute path.
		qml_root_str,                // directory to watch for qml file changes
		&this->qml_info              // qml data: Engine *, the data directory, ...
	);
	*/
	//// -- gui initialization

	while (not this->window->should_close()) {
		//this->renderer->render(pass1);

		this->window->update();

		this->renderer->check_error();
	}
}

} // openage::presenter
