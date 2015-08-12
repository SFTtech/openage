// Copyright 2014-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <string>
#include <ctime>

#include "coord/window.h"

namespace openage {
namespace renderer {
class Renderer;
}

class ScreenshotManager {
public:
	ScreenshotManager(renderer::Renderer *renderer);
	~ScreenshotManager();

	/**
	 * To be called to save a screenshot.
	 */
	void save_screenshot();

private:
	/**
	 * To be called to get the next screenshot filename into the array.
	 */
	std::string gen_next_filename();

	/**
	 * Contains the number to be in the next screenshot filename.
	 */
	unsigned count;

	/**
	 * Contains the last time when a screenshot was taken.
	 */
	std::time_t last_time;

	/**
	 * The renderer where to take screenshots from.
	 */
	class renderer::Renderer *renderer;
};

} // openage
