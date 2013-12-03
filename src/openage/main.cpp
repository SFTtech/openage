#include "main.h"

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <unistd.h>

#include "../engine/engine.h"
#include "../engine/init.h"
#include "../engine/log.h"
#include "../engine/util/timer.h"
#include "../engine/console/init.h"
#include "../engine/console/console.h"

#include "init.h"
#include "callbacks.h"
#include "gamestate.h"
#include "objects/terrain.h"
#include "objects/building.h"

namespace openage {

int main() {
	engine::util::Timer timer;

	//init engine
	timer.start();
	engine::init("openage");
	//init engine::console
	engine::console::init();
        engine::console::bgcol = engine::util::col {255, 255, 255, 180};
	engine::console::fgcol = engine::util::col {0, 0, 0, 255};

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
