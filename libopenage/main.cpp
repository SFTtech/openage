// Copyright 2015-2023 the openage authors. See copying.md for legal info.

#include "main.h"

#include <memory>

#include "cvar/cvar.h"
#include "engine/engine.h"
#include "util/timer.h"

namespace openage {

/*
 * This function is called from Cython, see main_cpp.pyx.
 *
 * This is the main entry point to the C++ part.
 */
int run_game(const main_arguments &args) {
	// TODO: store args.fps_limit and args.gl_debug as default in the cvar system.

	util::Timer timer;
	timer.start();

	// read and apply the configuration files
	auto cvar_manager = std::make_shared<cvar::CVarManager>(args.root_path["cfg"]);
	cvar_manager->load_all();

	// set engine run_mode
	openage::engine::Engine::mode run_mode = openage::engine::Engine::mode::FULL;
	if (args.headless) {
		run_mode = openage::engine::Engine::mode::HEADLESS;
	}

	openage::engine::Engine engine{run_mode, args.root_path, args.mods};

	engine.loop();

	return 0;
}

} // namespace openage
