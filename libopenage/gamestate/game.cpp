// Copyright 2018-2023 the openage authors. See copying.md for legal info.

#include "game.h"

#include <nyan/database.h>
#include <nyan/file.h>

#include "assets/mod_manager.h"
#include "event/time_loop.h"
#include "gamestate/game_state.h"
#include "gamestate/universe.h"

namespace openage::gamestate {

Game::Game(const util::Path &root_dir,
           const std::shared_ptr<openage::event::EventLoop> &event_loop,
           const std::shared_ptr<assets::ModManager> &mod_manager) :
	db{nyan::Database::create()},
	state{std::make_shared<GameState>(this->db, event_loop)},
	universe{std::make_shared<Universe>(root_dir, state)} {
	this->load_data(mod_manager);
}

const std::shared_ptr<GameState> &Game::get_state() const {
	return this->state;
}

void Game::attach_renderer(const std::shared_ptr<renderer::RenderFactory> &render_factory) {
	this->universe->attach_renderer(render_factory);
}

void Game::load_data(const std::shared_ptr<assets::ModManager> &mod_manager) {
	auto load_order = mod_manager->get_load_order();

	for (auto &mod_id : load_order) {
		auto mod = mod_manager->get_modpack(mod_id);
		auto info = mod->get_info();

		auto includes = info.includes;
		for (const auto &include : includes) {
			// handle wildcards
			auto parts = util::split(include, '/');
			auto last_part = parts.back();
			bool recursive = false;
			auto search = include;
			if (last_part == "**") {
				recursive = true;
				search = include.substr(0, include.size() - 2);
			}

			this->load_path(info.path.get_parent(), info.path.get_name(), search, recursive);
		}
	}
}

void Game::load_path(const util::Path &base_dir,
                     const std::string &mod_dir,
                     const std::string &search,
                     bool recursive) {
	auto base_path = base_dir.resolve_native_path();
	auto search_path = base_dir / mod_dir / search;

	auto fileload_func = [&base_path, &mod_dir](const std::string &filename) {
		// nyan wants a string filepath, so we have to construct it from the
		// path and subpath parameters
		return std::make_shared<nyan::File>(base_path + "/" + filename);
	};

	// file loading
	if (search_path.is_file() && search_path.get_suffix() == ".nyan") {
		this->db->load(mod_dir + "/" + search, fileload_func);
		log::log(INFO << "Loaded .nyan file: " << search_path);
		return;
	}

	// directory loading
	if (search_path.is_dir()) {
		// load all files in a directory
		for (auto p : search_path.iterdir()) {
			if (p.is_dir() and not recursive) {
				// folders are skipped unless we read recursively
				continue;
			}

			auto new_search = search + "/" + p.get_name();
			this->load_path(base_dir, mod_dir, new_search, recursive);
		}
	}
}

} // namespace openage::gamestate
