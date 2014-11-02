// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_SCREENSHOT_H_
#define OPENAGE_SCREENSHOT_H_

#include <SDL2/SDL.h>
#include "coord/window.h"

#include <ctime>

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
	char *gen_next_filename();

	/** contains the number to be in the next screenshot filename */
	unsigned count;

	/** contains the last time when a screenshot was taken */
	std::time_t last_time;

};

} //namespace openage

#endif
