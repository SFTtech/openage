// Copyright 2014-2017 the openage authors. See copying.md for legal info.

#include <cstdio>
#include <cstring>
#include <ctime>
#include <memory>

#include "coord/window.h"
#include "job/job_manager.h"
#include "log/log.h"

#include <ctime>
#include "screenshot.h"
#include "util/strings.h"
#include "renderer/renderer.h"

namespace openage {


ScreenshotManager::ScreenshotManager(job::JobManager *job_mgr, renderer::Renderer *renderer)
	:
	count{0},
    job_manager{job_mgr},
    renderer{renderer} {}


ScreenshotManager::~ScreenshotManager() {}


std::string ScreenshotManager::gen_next_filename() {
	std::time_t t = std::time(NULL);

	if (t == this->last_time) {
		this->count++;
	} else {
		this->count = 0;
		this->last_time = t;
	}

	constexpr const size_t timestamp_size = 32;

	char timestamp[timestamp_size];
	std::strftime(timestamp, timestamp_size,
	              "%Y-%m-%d_%H-%M-%S", std::localtime(&t));

	return util::sformat("/tmp/openage_%s_%02d.png", timestamp, this->count);
}


void ScreenshotManager::save_screenshot() {
    this->job_manager->enqueue<bool>([this] () {
        std::string filename = this->gen_next_filename();
        this->renderer->screenshot(filename);
        return true;
    });
}

} // openage
