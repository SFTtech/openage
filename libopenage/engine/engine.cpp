// Copyright 2013-2021 the openage authors. See copying.md for legal info.

#include "engine.h"

#include "event/loop.h"

// TODO
#include "gamestate/game.h"

namespace openage {
namespace engine {

Engine::Engine(enum mode mode,
               const util::Path &root_dir,
               const std::shared_ptr<cvar::CVarManager> &cvar_manager) :
	running{false},
	run_mode{mode},
	root_dir{root_dir},
	cvar_manager{cvar_manager},
	event_loop{std::make_shared<event::Loop>()},
	game{std::make_shared<gamestate::Game>(root_dir)} {
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

void Engine::attach_renderer(const std::shared_ptr<renderer::RenderFactory> &render_factory) {
	this->game->attach_renderer(render_factory);
}

} // namespace engine
} // namespace openage