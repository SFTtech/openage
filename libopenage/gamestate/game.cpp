// Copyright 2018-2023 the openage authors. See copying.md for legal info.

#include "game.h"

#include "event/time_loop.h"
#include "gamestate/game_state.h"
#include "gamestate/universe.h"

namespace openage::gamestate {

Game::Game(const util::Path &root_dir,
           const std::shared_ptr<openage::event::EventLoop> &event_loop) :
	state{std::make_shared<GameState>(event_loop)},
	universe{std::make_shared<Universe>(root_dir, state)} {
}

const std::shared_ptr<GameState> &Game::get_state() const {
	return this->state;
}

void Game::attach_renderer(const std::shared_ptr<renderer::RenderFactory> &render_factory) {
	this->universe->attach_renderer(render_factory);
}

} // namespace openage::gamestate
