// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "main.h"

#include "console/console.h"
#include "gamedata/color.gen.h"
#include "util/file.h"

#include "engine.h"
#include "game_main.h"

namespace openage {


int run_game(const main_arguments &args) {
	log::log(MSG(info) << "launching engine with data directory '" << args.data_directory << "'");

	util::Timer timer;
	timer.start();

	util::Dir data_dir{args.data_directory.c_str()};

	Engine::create(&data_dir, "openage");
	Engine &engine = Engine::get();

	// initialize terminal colors
	auto termcolors = util::read_csv_file<gamedata::palette_color>(data_dir.join("converted/termcolors.docx"));

	console::Console console;
	console.load_colors(termcolors);
	console.register_to_engine(&engine);

	log::log(MSG(info).fmt("Loading time [engine]: %5.3f s", timer.getval() / 1.0e9));

	timer.start();

	{
		// create a game that uses the engine.
		GameMain game{&engine};

		log::log(MSG(info).fmt("Loading time   [game]: %5.3f s", timer.getval() / 1.0e9));

		// run main loop
		engine.run();
	}

	Engine::destroy();

	return 0;
}


} // openage
