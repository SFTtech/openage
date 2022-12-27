// Copyright 2015-2021 the openage authors. See copying.md for legal info.

#include "main.h"

#include <thread>

#include "cvar/cvar.h"
#include "engine/engine.h"
#include "log/log.h"
#include "presenter/presenter.h"
#include "util/timer.h"

namespace openage {

/*
 * This function is called from Cython, see main_cpp.pyx.
 *
 * This is the main entry point to the C++ part.
 */
int run_game(const main_arguments &args) {
	log::log(MSG(info)
	         << "launching engine with "
	         << args.root_path
	         << " and fps limit "
	         << args.fps_limit);

	// TODO: store args.fps_limit and args.gl_debug as default in the cvar system.

	util::Timer timer;
	timer.start();

	// read and apply the configuration files
	auto cvar_manager = std::make_shared<cvar::CVarManager>(args.root_path["cfg"]);
	cvar_manager->load_all();

	// TODO: select run_mode by launch argument
	openage::engine::Engine::mode run_mode = engine::Engine::mode::FULL;

	auto engine = std::make_shared<engine::Engine>(run_mode, args.root_path, cvar_manager);
	auto presenter = std::make_shared<presenter::Presenter>(args.root_path, engine);

	std::jthread engine_thread([&]() {
		engine->run();

		engine.reset();
	});
	std::jthread presenter_thread([&]() {
		presenter->run();

		// Make sure that the presenter gets destructed in the same thread
		// otherwise OpenGL complains about missing contexts
		presenter.reset();
	});

	return 0;
}

} // namespace openage
