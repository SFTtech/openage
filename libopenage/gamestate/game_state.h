// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <unordered_map>

#include "event/state.h"
#include "gamestate/types.h"


namespace nyan {
class Database;
class View;
} // namespace nyan

namespace openage {

namespace assets {
class ModManager;
}

namespace event {
class EventLoop;
}

namespace gamestate {
class GameEntity;
class Player;
class Terrain;

/**
 * State of the game.
 *
 * Contains index structures for looking up game entities and other
 * information for the current game.
 */
class GameState : public openage::event::State {
public:
	/**
     * Create a new game state.
     *
     * @param db Nyan game data database.
     * @param event_loop Event loop for the game state.
     */
	explicit GameState(const std::shared_ptr<nyan::Database> &db,
	                   const std::shared_ptr<openage::event::EventLoop> &event_loop);

	/**
     * Get the nyan database view for the whole game.
     *
     * Players have individual views for their own data.
     *
     * @return nyan database view.
     */
	const std::shared_ptr<nyan::View> &get_db_view();

	/**
     * Add a new game entity to the index.
     *
     * @param entity New game entity.
     */
	void add_game_entity(const std::shared_ptr<GameEntity> &entity);

	/**
     * Add a new player to the index.
     *
     * @param player New player.
     */
	void add_player(const std::shared_ptr<Player> &player);

	/**
     * Set the terrain of the current game.
     *
     * @param terrain Terrain object.
     */
	void set_terrain(const std::shared_ptr<Terrain> &terrain);

	/**
     * Get a game entity by its ID.
     *
     * @param id ID of the game entity.
	 *
     * @return Game entity with the given ID.
     */
	const std::shared_ptr<GameEntity> &get_game_entity(entity_id_t id) const;

	/**
     * Get all game entities in the current game.
     *
     * @return Map of all game entities in the current game by their ID.
     */
	const std::unordered_map<entity_id_t, std::shared_ptr<GameEntity>> &get_game_entities() const;

	/**
	 * Get a player by its ID.
	 *
	 * @param id ID of the player.
	 *
	 * @return Player with the given ID.
	 */
	const std::shared_ptr<Player> &get_player(player_id_t id) const;

	/**
     * Get the terrain of the current game.
     *
     * @return Terrain object.
     */
	const std::shared_ptr<Terrain> &get_terrain() const;

	/**
     * TODO: Only for testing.
     */
	const std::shared_ptr<assets::ModManager> &get_mod_manager() const;
	void set_mod_manager(const std::shared_ptr<assets::ModManager> &mod_manager);

private:
	/**
     * View for the nyan game data database.
     */
	std::shared_ptr<nyan::View> db_view;

	/**
     * Map of all game entities in the current game by their ID.
     */
	std::unordered_map<entity_id_t, std::shared_ptr<GameEntity>> game_entities;

	/**
     * Map of all players in the current game by their ID.
     */
	std::unordered_map<player_id_t, std::shared_ptr<Player>> players;

	/**
     * Terrain of the current game.
     */
	std::shared_ptr<Terrain> terrain;

	/**
     * TODO: Only for testing
     */
	std::shared_ptr<assets::ModManager> mod_manager;
};
} // namespace gamestate
} // namespace openage
