// Copyright 2018-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>

#include "gamestate/types.h"


namespace nyan {
class View;
} // namespace nyan

namespace openage::gamestate {

/**
 * Entity for managing a player inside the game world.
 */
class Player {
public:
	/**
	 * Create a new player with the given id.
	 *
	 * @param id Unique player ID.
	 * @param db_view View of the nyan database used for the player.
	 */
	Player(player_id_t id,
	       const std::shared_ptr<nyan::View> &db_view);

	// players can't be copied to prevent duplicate IDs
	Player(const Player &) = delete;
	Player(Player &&) = default;

	Player &operator=(const Player &) = delete;
	Player &operator=(Player &&) = default;

	~Player() = default;

	/**
	 * Get the unique ID of the player.
	 *
	 * @return Unique player ID.
	 */
	player_id_t get_id() const;

	/**
	 * Get the nyan game database view for the player.
	 *
	 * @return nyan database view.
	 */
	const std::shared_ptr<nyan::View> &get_db_view() const;

private:
	/**
	 * Player ID. Must be unique.
	 */
	const player_id_t id;

	/**
     * Player view of the nyan game data database.
     */
	std::shared_ptr<nyan::View> db_view;
};

} // namespace openage::gamestate
