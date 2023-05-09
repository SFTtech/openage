// Copyright 2013-2023 the openage authors. See copying.md for legal info.

#include "engine.h"

#include "event/loop.h"
#include "event/simulation.h"

#include "gamestate/entity_factory.h"
#include "gamestate/event/spawn_entity.h"

// TODO
#include "gamestate/game.h"

namespace openage {
namespace engine {

Engine::Engine(enum mode mode,
               const util::Path &root_dir,
               const std::shared_ptr<cvar::CVarManager> &cvar_manager,
               const std::shared_ptr<event::Simulation> simulation) :
	running{false},
	run_mode{mode},
	root_dir{root_dir},
	cvar_manager{cvar_manager},
	simulation{simulation},
	entity_factory{std::make_shared<gamestate::EntityFactory>()},
	spawner{std::make_shared<gamestate::event::Spawner>(this->simulation->get_loop())},
	game{std::make_shared<gamestate::Game>(root_dir, this->simulation)} {
	log::log(MSG(info) << "Created engine");
}


Engine::~Engine() {}


void Engine::run() {
	this->start();
	while (this->running) {
		// TODO: Do something
	}
	log::log(MSG(info) << "engine loop exited");
}


void Engine::start() {
	std::unique_lock lock{this->mutex};

	this->init_event_handlers();

	this->running = true;

	log::log(MSG(info) << "Engine started");
}


void Engine::stop() {
	std::unique_lock lock{this->mutex};

	this->running = false;

	log::log(MSG(info) << "Engine stopped");
}


const util::Path &Engine::get_root_dir() {
	return this->root_dir;
}

const std::shared_ptr<cvar::CVarManager> Engine::get_cvar_manager() {
	return this->cvar_manager;
}

const std::shared_ptr<gamestate::Game> Engine::get_game() {
	return this->game;
}

const std::shared_ptr<gamestate::event::Spawner> Engine::get_spawner() {
	return this->spawner;
}

void Engine::attach_renderer(const std::shared_ptr<renderer::RenderFactory> &render_factory) {
	this->game->attach_renderer(render_factory);
	this->entity_factory->attach_renderer(render_factory);
}

void Engine::init_event_handlers() {
	auto handler = std::make_shared<gamestate::event::SpawnEntityHandler>(this->simulation->get_loop(),
	                                                                      this->entity_factory,
	                                                                      this->root_dir["assets/test/textures/test_tank_mirrored.sprite"]);
	this->simulation->get_loop()->add_event_class(handler);
}

} // namespace engine
} // namespace openage
