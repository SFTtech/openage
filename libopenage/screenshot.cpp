// Copyright 2014-2016 the openage authors. See copying.md for legal info.

#include <SDL2/SDL.h>
#include <ctime>

#include "coord/window.h"
#include "log/log.h"
#include "log/message.h"
#include <ctime>
#include "screenshot.h"
#include "util/strings.h"
#include "renderer/renderer.h"

namespace openage {


ScreenshotManager::ScreenshotManager(renderer::Renderer *renderer)
	:
	count{0},
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
	std::string filename = this->gen_next_filename();

	this->renderer->screenshot(filename);
}


} // openage
