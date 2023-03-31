// Copyright 2018-2023 the openage authors. See copying.md for legal info.

#include "game.h"

#include "event/simulation.h"
#include "gamestate/game_state.h"
#include "gamestate/universe.h"

namespace openage::gamestate {

Game::Game(const util::Path &root_dir,
           const std::shared_ptr<openage::event::Simulation> &simulation) :
	state{std::make_shared<GameState>(simulation->get_loop())},
	universe{std::make_shared<Universe>(root_dir, state)},
	simulation{simulation} {
}

const std::shared_ptr<GameState> &Game::get_state() const {
	return this->state;
}

void Game::attach_renderer(const std::shared_ptr<renderer::RenderFactory> &render_factory) {
	this->universe->attach_renderer(render_factory);
}

} // namespace openage::gamestate
