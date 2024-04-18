// Copyright 2013-2024 the openage authors. See copying.md for legal info.

#include "simulation.h"

#include "assets/mod_manager.h"
#include "event/event_loop.h"
#include "gamestate/entity_factory.h"
#include "gamestate/event/drag_select.h"
#include "gamestate/event/process_command.h"
#include "gamestate/event/send_command.h"
#include "gamestate/event/spawn_entity.h"
#include "gamestate/event/wait.h"
#include "gamestate/terrain_factory.h"
#include "time/clock.h"
#include "time/time_loop.h"

// TODO
#include "gamestate/game.h"
#include "gamestate/game_state.h"

namespace openage::gamestate {

GameSimulation::GameSimulation(const util::Path &root_dir,
                               const std::shared_ptr<cvar::CVarManager> &cvar_manager,
                               const std::shared_ptr<openage::time::TimeLoop> time_loop) :
	running{false},
	root_dir{root_dir},
	cvar_manager{cvar_manager},
	time_loop{time_loop},
	event_loop{std::make_shared<openage::event::EventLoop>()},
	entity_factory{std::make_shared<gamestate::EntityFactory>()},
	terrain_factory{std::make_shared<gamestate::TerrainFactory>()},
	mod_manager{std::make_shared<assets::ModManager>(this->root_dir / "assets" / "converted")},
	spawner{std::make_shared<gamestate::event::Spawner>(this->event_loop)},
	commander{std::make_shared<gamestate::event::Commander>(this->event_loop)} {
	auto mods = mod_manager->enumerate_modpacks(root_dir / "assets" / "converted");
	for (const auto &mod : mods) {
		this->mod_manager->register_modpack(mod);
	}

	log::log(MSG(info) << "Created game simulation");
}


void GameSimulation::run() {
	this->start();
	while (this->running) {
		time::time_t current_time = this->time_loop->get_clock()->get_time();
		this->event_loop->reach_time(current_time, this->game->get_state());
	}
	log::log(MSG(info) << "Game simulation loop exited");
}


void GameSimulation::start() {
	std::unique_lock lock{this->mutex};

	this->init_event_handlers();

	// TODO: wait for presenter to initialize before starting?
	this->game = std::make_shared<gamestate::Game>(event_loop,
	                                               this->mod_manager,
	                                               this->entity_factory,
	                                               this->terrain_factory);

	this->running = true;

	log::log(MSG(info) << "Game simulation started");
}


void GameSimulation::stop() {
	std::unique_lock lock{this->mutex};

	this->running = false;

	log::log(MSG(info) << "Game simulation stopped");
}


const util::Path &GameSimulation::get_root_dir() {
	std::shared_lock lock{this->mutex};

	return this->root_dir;
}

const std::shared_ptr<cvar::CVarManager> GameSimulation::get_cvar_manager() {
	std::shared_lock lock{this->mutex};

	return this->cvar_manager;
}

const std::shared_ptr<gamestate::Game> GameSimulation::get_game() {
	std::shared_lock lock{this->mutex};

	return this->game;
}

const std::shared_ptr<openage::event::EventLoop> GameSimulation::get_event_loop() {
	std::shared_lock lock{this->mutex};

	return this->event_loop;
}

const std::shared_ptr<gamestate::event::Spawner> GameSimulation::get_spawner() {
	std::shared_lock lock{this->mutex};

	return this->spawner;
}

const std::shared_ptr<gamestate::event::Commander> GameSimulation::get_commander() {
	std::shared_lock lock{this->mutex};

	return this->commander;
}

void GameSimulation::attach_renderer(const std::shared_ptr<renderer::RenderFactory> &render_factory) {
	std::unique_lock lock{this->mutex};

	this->game->attach_renderer(render_factory);
	this->entity_factory->attach_renderer(render_factory);
	this->terrain_factory->attach_renderer(render_factory);
}

void GameSimulation::set_modpacks(const std::vector<std::string> &modpacks) {
	std::unique_lock lock{this->mutex};

	std::vector<std::string> mods{"engine"};
	mods.insert(mods.end(), modpacks.begin(), modpacks.end());

	this->mod_manager->activate_modpacks(mods);

	// TODO: Prevent setting modpacks if a game is already running
}

void GameSimulation::init_event_handlers() {
	auto drag_select_handler = std::make_shared<gamestate::event::DragSelectHandler>();
	auto spawn_handler = std::make_shared<gamestate::event::SpawnEntityHandler>(this->event_loop,
	                                                                            this->entity_factory);
	auto command_handler = std::make_shared<gamestate::event::SendCommandHandler>();
	auto manager_handler = std::make_shared<gamestate::event::ProcessCommandHandler>();
	auto wait_handler = std::make_shared<gamestate::event::WaitHandler>();
	this->event_loop->add_event_handler(drag_select_handler);
	this->event_loop->add_event_handler(spawn_handler);
	this->event_loop->add_event_handler(command_handler);
	this->event_loop->add_event_handler(manager_handler);
	this->event_loop->add_event_handler(wait_handler);
}

} // namespace openage::gamestate
