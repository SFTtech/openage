// Copyright 2018-2023 the openage authors. See copying.md for legal info.

#include "game.h"

#include <vector>

#include <nyan/nyan.h>

#include "log/log.h"
#include "log/message.h"

#include "assets/mod_manager.h"
#include "assets/modpack.h"
#include "gamestate/entity_factory.h"
#include "gamestate/game_state.h"
#include "gamestate/terrain.h"
#include "gamestate/terrain_factory.h"
#include "gamestate/universe.h"
#include "util/path.h"
#include "util/strings.h"


namespace openage::gamestate {

Game::Game(const std::shared_ptr<openage::event::EventLoop> &event_loop,
           const std::shared_ptr<assets::ModManager> &mod_manager,
           const std::shared_ptr<EntityFactory> &entity_factory,
           const std::shared_ptr<TerrainFactory> &terrain_factory) :
	db{nyan::Database::create()},
	state{std::make_shared<GameState>(this->db, event_loop)},
	universe{std::make_shared<Universe>(state)} {
	this->load_data(mod_manager);

	// TODO: Testing player creation
	auto player1 = entity_factory->add_player(event_loop, state, "");
	auto player2 = entity_factory->add_player(event_loop, state, "");
	state->add_player(player1);
	state->add_player(player2);

	// TODO: This lets the spawner event check which modpacks are loaded,
	//       so that it can decide which entities it can spawn.
	//       This can be removed when we spawn based on game logic rather than
	//       hardcoded entity types.
	this->state->set_mod_manager(mod_manager);

	this->generate_terrain(terrain_factory);
}

const std::shared_ptr<GameState> &Game::get_state() const {
	return this->state;
}

void Game::attach_renderer(const std::shared_ptr<renderer::RenderFactory> &render_factory) {
	this->universe->attach_renderer(render_factory);
	this->state->get_terrain()->attach_renderer(render_factory);
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
				if (parts.size() == 1) {
					// include = "**"
					// start in root directory
					search = "";
				}
				else {
					// include = "path/to/somewhere/**"
					// remove the wildcard '**' and the slash '/'
					search = include.substr(0, include.size() - 3);
				}
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
		log::log(INFO << "Loading .nyan file: " << filename);
		auto loc = base_path + "/" + filename;
		return std::make_shared<nyan::File>(loc);
	};

	// file loading
	if (search_path.is_file() && search_path.get_suffix() == ".nyan") {
		auto loc = mod_dir + "/" + search;
		this->db->load(loc, fileload_func);
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

void Game::generate_terrain(const std::shared_ptr<TerrainFactory> &terrain_factory) {
	auto terrain = terrain_factory->add_terrain();

	auto chunk0 = terrain_factory->add_chunk(util::Vector2s{10, 10}, util::Vector2s{0, 0});
	auto chunk1 = terrain_factory->add_chunk(util::Vector2s{10, 10}, util::Vector2s{10, 0});
	terrain->add_chunk(chunk0);
	terrain->add_chunk(chunk1);

	this->state->set_terrain(terrain);
}

} // namespace openage::gamestate
