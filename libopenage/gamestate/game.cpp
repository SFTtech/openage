// Copyright 2018-2023 the openage authors. See copying.md for legal info.

#include "game.h"

#include <nyan/database.h>
#include <nyan/file.h>

#include "event/time_loop.h"
#include "gamestate/game_state.h"
#include "gamestate/universe.h"

namespace openage::gamestate {

Game::Game(const util::Path &root_dir,
           const std::shared_ptr<openage::event::EventLoop> &event_loop) :
	db{nyan::Database::create()},
	state{std::make_shared<GameState>(this->db, event_loop)},
	universe{std::make_shared<Universe>(root_dir, state)} {
	auto nyan_dir = root_dir / "assets" / "converted" / "engine";
	auto base_path = nyan_dir.get_parent().resolve_native_path();
	for (auto f : nyan_dir.iterdir()) {
		if (f.is_file() && f.get_suffix() == ".nyan") {
			this->db->load(nyan_dir.get_name() + "/" + f.get_name(),
			               [&base_path](const std::string &filename) {
							   return std::make_shared<nyan::File>(base_path + "/" + filename);
						   });
		}
	}
}

const std::shared_ptr<GameState> &Game::get_state() const {
	return this->state;
}

void Game::attach_renderer(const std::shared_ptr<renderer::RenderFactory> &render_factory) {
	this->universe->attach_renderer(render_factory);
}

} // namespace openage::gamestate
