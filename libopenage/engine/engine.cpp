// Copyright 2013-2021 the openage authors. See copying.md for legal info.

#include "engine.h"

namespace openage {
namespace engine {

Engine::Engine(enum mode mode,
               const util::Path &root_dir,
               const std::shared_ptr<cvar::CVarManager> &cvar_manager) :
	running{false},
	run_mode{mode},
	root_dir{root_dir},
	cvar_manager{cvar_manager},
	qml_info{this, this->root_dir["assets"]} {
}


Engine::~Engine() {}


void Engine::run() {
	this->running = true;
}


void Engine::stop() {
	this->running = false;
}


const util::Path &Engine::get_root_dir() {
	return this->root_dir;
}

std::shared_ptr<cvar::CVarManager> Engine::get_cvar_manager() {
	return this->cvar_manager;
}

renderer::gui::QMLInfo Engine::get_qml_info() {
	return this->qml_info;
}

} // namespace engine
} // namespace openage
