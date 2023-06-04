// Copyright 2018-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "util/path.h"

namespace nyan {
class Database;
}

namespace openage {

namespace assets {
class ModManager;
}

namespace event {
class EventLoop;
}

namespace renderer {
class RenderFactory;
}

namespace gamestate {
class GameState;
class Universe;

/**
 * Manages a game session (settings, win conditions, etc.).
 *
 * TODO: Create sensible structure of gamestate classes. Right now it's
 *
 *       Game->Universe|->World->GameEntity
 *                     |->Terrain
 *
 * 		which can be confusing.
 */
class Game {
public:
	/**
	 * Create a new game.
	 *
	 * @param root_dir openage root directory.
     * @param event_loop Event simulation loop for the gamestate.
     * @param mod_manager Mod manager.
	 */
	Game(const util::Path &root_dir,
	     const std::shared_ptr<openage::event::EventLoop> &event_loop,
	     const std::shared_ptr<assets::ModManager> &mod_manager);
	~Game() = default;

	/**
     * Get the current game state.
     */
	const std::shared_ptr<GameState> &get_state() const;

	/**
	 * Attach a renderer to the game which enables graphical display options for
	 * all ingame entities.
	 *
	 * @param render_factory Factory for creating connector objects for gamestate->renderer
	 *                       communication.
	 */
	void attach_renderer(const std::shared_ptr<renderer::RenderFactory> &render_factory);

private:
	/**
     * Load game data from the filesystem.
     *
     * @param mod_manager Mod manager.
     */
	void load_data(const std::shared_ptr<assets::ModManager> &mod_manager);

	/**
     * Load game data from the filesystem recursively.
     *
     * TODO: Move this into nyan.
     *
     * @param base_dir Base directory where mods are stored.
     * @param mod_dir Name of the mod directory.
     * @param search Search path relative to the mod directory.
     * @param recursive if true, recursively search subfolders if the the search path is a directory.
     */
	void load_path(const util::Path &base_dir,
	               const std::string &mod_dir,
	               const std::string &search,
	               bool recursive = false);

	/**
     * Nyan game data database.
     */
	std::shared_ptr<nyan::Database> db;

	/**
     * State of the current game.
     */
	std::shared_ptr<GameState> state;

	/**
	 * Object that controls entities in the game world.
	 */
	std::shared_ptr<Universe> universe;
};

} // namespace gamestate
} // namespace openage
