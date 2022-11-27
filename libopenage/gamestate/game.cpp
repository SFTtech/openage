// Copyright 2018-2018 the openage authors. See copying.md for legal info.

#include "game.h"

#include "gamestate/universe.h"

namespace openage::gamestate {

Game::Game(const util::Path &root_dir) :
	universe{std::make_shared<Universe>(root_dir)} {
}

void Game::attach_renderer(const std::shared_ptr<renderer::RenderFactory> &render_factory) {
	this->universe->attach_renderer(render_factory);
}

} // namespace openage::gamestate
