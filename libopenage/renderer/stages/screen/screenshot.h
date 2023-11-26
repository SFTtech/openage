// Copyright 2014-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <ctime>
#include <memory>
#include <string>

#include "util/path.h"


namespace openage {

namespace job {
class JobManager;
}

namespace renderer::screen {
class ScreenRenderStage;

/**
 * Takes screenshots, duh.
 */
class ScreenshotManager {
public:
	/**
	 * Create a new screenshot manager.
	 *
	 * @param renderer Screen render stage to take the screenshot from.
	 * @param outdir Directory where the screenshots are saved.
	 * @param job_mgr Job manager to use for writing the screenshot to disk.
	 */
	ScreenshotManager(std::shared_ptr<ScreenRenderStage> &renderer,
	                  util::Path &outdir,
	                  std::shared_ptr<job::JobManager> &job_mgr);

	~ScreenshotManager() = default;

	/**
	 * Generate and save a screenshot of the last frame.
	 */
	void save_screenshot();

private:
	/**
	 * Generates a filename for the screenshot.
	 *
	 * @return Filename for the screenshot.
	 */
	std::string gen_next_filename();

	/**
	 * Directory where the screenshots are saved.
	 */
	util::Path outdir;

	/**
	 * Counter for the screenshot filename. Used if multiple screenshots
	 * are taken in the same second.
	 */
	unsigned count;

	/**
	 * Last time when a screenshot was taken.
	 */
	std::time_t last_time;

	/**
	 * Screen render stage to take the screenshot from.
	 */
	std::shared_ptr<ScreenRenderStage> renderer;

	/**
	 * Job manager to use for writing the screenshot to disk.
	 */
	std::shared_ptr<job::JobManager> job_manager;
};

} // namespace renderer::screen
} // namespace openage
