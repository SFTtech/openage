// Copyright 2015-2021 the openage authors. See copying.md for legal info.

#include "main.h"

#include "console/console.h"
#include "cvar/cvar.h"
#include "engine.h"
#include "gamedata/color_dummy.h"
#include "log/log.h"
#include "presenter/legacy/game_control.h"
#include "presenter/legacy/legacy_renderer.h"
#include "shader/program.h"
#include "shader/shader.h"
#include "util/file.h"
#include "util/timer.h"

namespace openage {

/*
 * This function is called from Cython, see main_cpp.pyx.
 *
 * This is the main entry point to the C++ part.
 */
int run_game(const main_arguments &args) {
	log::log(MSG(info)
	         << "launching legacy engine with "
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
	openage::Engine::mode run_mode = Engine::mode::FULL;

	if (run_mode != Engine::mode::LEGACY) {
		auto presenter = presenter::Presenter(args.root_path);
		presenter.run();
	}

	return 0;
}

} // namespace openage
