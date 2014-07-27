#include "main.h"

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../engine/engine.h"
#include "../engine/init.h"
#include "../engine/log.h"
#include "../engine/util/timer.h"
#include "../engine/console/console.h"
#include "../args.h"

#include "init.h"
#include "callbacks.h"
#include "gamestate.h"

namespace openage {

int main() {
	engine::util::Timer timer;

	struct stat data_dir_stat;

	if (stat(args.data_directory, &data_dir_stat) == -1) {
		throw engine::Error("Failed checking directory %s: %s", args.data_directory, strerror(errno));
	}

	/*
	constexpr size_t max_dirname_length = 2048;
	char current_dir_name[max_dirname_length];

	if (NULL == getcwd(current_dir_name, max_dirname_length)) {
		throw engine::Error("working dir filename longer than %lu.", max_dirname_length);
	}
	*/

	engine::log::msg("using data directory '%s'", args.data_directory);
	engine::util::Dir data_dir{args.data_directory};

	timer.start();
	engine::init(data_dir, "openage");

	//init engine::console
	auto termcolors = engine::util::read_csv_file<gamedata::palette_color>(data_dir.join("age/assets/termcolors.docx"));
	engine::console::init(termcolors);

	engine::log::msg("Loading time [engine]: %5.3f s", timer.getval() / 1000.f);

	//init game
	timer.start();
	openage::init(data_dir);
	engine::log::msg("Loading time   [game]: %5.3f s", timer.getval() / 1000.f);

	//run main loop
	engine::loop();

	//de-init
	openage::destroy();
	engine::console::destroy();
	engine::destroy();

	return 0;
}

} //namespace openage
