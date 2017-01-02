// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#include "main.h"

#include "console/console.h"
#include "gamedata/color.gen.h"
#include "util/file.h"

#include "engine.h"
#include "game_control.h"
#include "game_renderer.h"
#include "gamestate/generator.h"

#include "shader/shader.h"
#include "shader/program.h"

namespace openage {


int run_game(const main_arguments &args) {
	log::log(MSG(info) << "launching engine with data directory '"
	                   << args.data_directory
	                   << "' and fps limit "
	                   << args.fps_limit);

	util::Timer timer;
	timer.start();

	util::Dir data_dir{args.data_directory.c_str()};

	Engine engine{&data_dir, args.fps_limit, args.gl_debug, "openage"};

	// read and apply the configuration files
	auto &cvar_manager = engine.get_cvar_manager();
	cvar_manager.load_main_config();

	// initialize terminal colors
	std::vector<gamedata::palette_color> termcolors;
	util::read_csv_file(data_dir.join("converted/termcolors.docx"), termcolors);

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
