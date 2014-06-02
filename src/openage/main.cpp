#include "main.h"

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
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

	//init engine
	if (chdir(args.data_directory) == -1) {
		throw engine::Error("Failed to change directory: %s", strerror(errno));
	}

	timer.start();
	engine::init("openage");

	//init engine::console
	auto termcolors = engine::util::read_csv_file<gamedata::palette_color>("age/assets/termcolors.docx");
	engine::console::init(termcolors);

	engine::log::msg("Loading time [engine]: %5.3f s", timer.getval() / 1000.f);

	//init game
	timer.start();
	openage::init();
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
