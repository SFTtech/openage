// Copyright 2014-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <string>
#include <ctime>

#include <SDL2/SDL.h>
#include "coord/window.h"

namespace openage {

class ScreenshotManager {
public:
	ScreenshotManager();
	~ScreenshotManager();

	/** to be called to save a screenshot */
	void save_screenshot();

	/** size of the game window, in coord_sdl */
	coord::window window_size;


private:

	/** to be called to get the next screenshot filename into the array */
	std::string gen_next_filename();

	/** contains the number to be in the next screenshot filename */
	unsigned count;

	/** contains the last time when a screenshot was taken */
	std::time_t last_time;

};

} // openage
