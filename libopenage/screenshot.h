// Copyright 2014-2018 the openage authors. See copying.md for legal info.

#pragma once

#include <string>
#include <ctime>
#include <memory>

#include "coord/pixel.h"

namespace openage {

namespace job {
class JobManager;
}

class ScreenshotManager {
public:
	/**
	 * Initializes the screenshot manager with the given job manager.
	 */
	ScreenshotManager(job::JobManager* job_mgr);

	~ScreenshotManager();

	/** To be called to save a screenshot. */
	void save_screenshot(coord::viewport_delta size);

	/** To be called by the job manager. Returns true on success, false otherwise. */
	bool encode_png(std::shared_ptr<uint8_t> pxdata,
	                coord::viewport_delta size);

	/** size of the game window, in coord_sdl */
	coord::viewport_delta window_size;

private:

	/** to be called to get the next screenshot filename into the array */
	std::string gen_next_filename();

	/** contains the number to be in the next screenshot filename */
	unsigned count;

	/** contains the last time when a screenshot was taken */
	std::time_t last_time;

	/** the job manager this screenshot manager uses */
	job::JobManager *job_manager;
};

} // openage
