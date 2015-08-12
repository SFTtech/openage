// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "main.h"

#include "console/console.h"
#include "gamedata/color.gen.h"
#include "util/file.h"

#include "engine.h"
#include "game_control.h"
#include "game_renderer.h"
#include "generator.h"

namespace openage {


int run_game(const main_arguments &args) {
	log::log(MSG(info) << "launching engine with data directory '" << args.data_directory << "'");

	util::Timer timer;
	timer.start();

	util::Dir data_dir{args.data_directory.c_str()};

	Engine::create(&data_dir, "openage");
	Engine &engine = Engine::get();

	// initialize terminal colors
	std::vector<gamedata::palette_color> termcolors;
	util::read_csv_file(data_dir.join("converted/termcolors.docx"), termcolors);

	console::Console console;
	console.load_colors(termcolors);
	console.register_to_engine(&engine);

	log::log(MSG(info).fmt("Loading time [engine]: %5.3f s", timer.getval() / 1.0e9));

	timer.start();

	{
		// create components that use the engine.
		GameRenderer renderer{&engine};
		GameControl control{&engine};
		Generator generator{&engine};

		log::log(MSG(info).fmt("Loading time   [game]: %5.3f s", timer.getval() / 1.0e9));

		// run main loop
		engine.run();
	}

	Engine::destroy();

	return 0;
}


} // openage
