// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "main.h"

#include "console/console.h"
#include "engine.h"
#include "game_control.h"
#include "game_renderer.h"
#include "gamedata/color.gen.h"
#include "gamestate/generator.h"
#include "log/log.h"
#include "shader/program.h"
#include "shader/shader.h"
#include "util/file.h"


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

	util::Timer timer;
	timer.start();

	Engine engine{args.root_path, args.fps_limit, args.gl_debug, "openage"};

	// read and apply the configuration files
	engine.get_cvar_manager().load_all();

	// initialize terminal colors
	std::vector<gamedata::palette_color> termcolors = util::read_csv_file<gamedata::palette_color>(
		args.root_path["assets/converted/termcolors.docx"]
	);

	// TODO: move inside the engine
	// TODO: support multiple consoles
	console::Console console{&engine};
	console.load_colors(termcolors);
	console.register_to_engine();

	log::log(MSG(info).fmt("Loading time [engine]: %5.3f s", timer.getval() / 1.0e9));

	timer.start();

	{
		// create components that use the engine.
		GameRenderer renderer{&engine};

		log::log(MSG(info).fmt("Loading time   [game]: %5.3f s", timer.getval() / 1.0e9));

		// run main loop
		engine.run();
	}

	return 0;
}

} // openage
