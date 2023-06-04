// Copyright 2013-2023 the openage authors. See copying.md for legal info.

#include "simulation.h"

#include "event/clock.h"
#include "event/event_loop.h"
#include "event/time_loop.h"

#include "gamestate/entity_factory.h"
#include "gamestate/event/spawn_entity.h"

// TODO
#include "gamestate/game.h"
#include "gamestate/game_state.h"

namespace openage {
namespace gamestate {

GameSimulation::GameSimulation(enum mode mode,
                               const util::Path &root_dir,
                               const std::shared_ptr<cvar::CVarManager> &cvar_manager,
                               const std::shared_ptr<openage::event::TimeLoop> time_loop) :
	running{false},
	run_mode{mode},
	root_dir{root_dir},
	cvar_manager{cvar_manager},
	time_loop{time_loop},
	event_loop{std::make_shared<openage::event::EventLoop>()},
	entity_factory{std::make_shared<gamestate::EntityFactory>()},
	spawner{std::make_shared<gamestate::event::Spawner>(this->event_loop)},
	game{std::make_shared<gamestate::Game>(root_dir, event_loop)} {
	log::log(MSG(info) << "Created engine");
}


void GameSimulation::run() {
	this->start();
	while (this->running) {
		auto current_time = this->time_loop->get_clock()->get_time();
		this->event_loop->reach_time(current_time, this->game->get_state());
	}
	log::log(MSG(info) << "Game simulation loop exited");
}


void GameSimulation::start() {
	std::unique_lock lock{this->mutex};

	this->init_event_handlers();

	this->running = true;

	log::log(MSG(info) << "Game simulation started");
}


void GameSimulation::stop() {
	std::unique_lock lock{this->mutex};

	this->running = false;

	log::log(MSG(info) << "Game simulation stopped");
}


const util::Path &GameSimulation::get_root_dir() {
	return this->root_dir;
}

const std::shared_ptr<cvar::CVarManager> GameSimulation::get_cvar_manager() {
	return this->cvar_manager;
}

const std::shared_ptr<gamestate::Game> GameSimulation::get_game() {
	return this->game;
}

const std::shared_ptr<openage::event::EventLoop> GameSimulation::get_event_loop() {
	return this->event_loop;
}

const std::shared_ptr<gamestate::event::Spawner> GameSimulation::get_spawner() {
	return this->spawner;
}

void GameSimulation::attach_renderer(const std::shared_ptr<renderer::RenderFactory> &render_factory) {
	this->game->attach_renderer(render_factory);
	this->entity_factory->attach_renderer(render_factory);
}

void GameSimulation::init_event_handlers() {
	auto handler = std::make_shared<gamestate::event::SpawnEntityHandler>(this->event_loop,
	                                                                      this->entity_factory,
	                                                                      this->root_dir["assets/test/textures/test_tank_mirrored.sprite"]);
	this->event_loop->add_event_handler(handler);
}

} // namespace gamestate
} // namespace openage
