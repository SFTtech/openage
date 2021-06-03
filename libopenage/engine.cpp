// Copyright 2013-2021 the openage authors. See copying.md for legal info.

#include "engine.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <epoxy/gl.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "config.h"
#include "error/error.h"
#include "log/log.h"
#include "presenter/legacy/legacy.h"
#include "version.h"
#include "versions/compiletime.h"

namespace openage {

Engine::Engine(enum mode mode,
               const util::Path &root_dir,
               const std::shared_ptr<cvar::CVarManager> &cvar_manager)
	:
	running{false},
	mode{mode},
	root_dir{root_dir},
	job_manager{SDL_GetCPUCount()},
	qml_info{this, root_dir["assets"]},
	cvar_manager{cvar_manager},
	profiler{this},
	gui_link{} {

	if (mode == mode::LEGACY) {
		this->old_display = std::make_unique<presenter::LegacyDisplay>(root_dir);
		return;
	}

	// TODO: implement FULL and HEADLESS mode :)
}


Engine::~Engine() {}


void Engine::run() {
	try {
		this->job_manager.start();
		this->running = true;

		if (this->mode == mode::LEGACY) {
			this->old_display->loop();
		}

		this->running = false;
	} catch (...) {
		this->job_manager.stop();
		throw;
	}
}


void Engine::stop() {
	this->job_manager.stop();
	this->running = false;
}


const util::Path &Engine::get_root_dir() {
	return this->root_dir;
}


job::JobManager *Engine::get_job_manager() {
	return &this->job_manager;
}


cvar::CVarManager &Engine::get_cvar_manager() {
	return *this->cvar_manager;
}

} // openage
