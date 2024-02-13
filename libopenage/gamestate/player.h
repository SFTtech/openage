// Copyright 2018-2024 the openage authors. See copying.md for legal info.

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

	Player(Player &&) = default;
	Player &operator=(Player &&) = default;

	~Player() = default;

	/**
	 * Copy this player.
	 *
	 * @param id Unique identifier.
	 *
	 * @return Copy of this player.
	 */
	std::shared_ptr<Player> copy(entity_id_t id);

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

protected:
	/**
	 * A player cannot be default copied because of their unique ID.
	 *
	 * \p copy() must be used instead.
	 */
	Player(const Player &) = default;
	Player &operator=(const Player &) = default;

private:
	/**
	 * Set the unique identifier of this player.
	 *
	 * Only called by \p copy().
	 *
	 * @param id New ID.
	 */
	void set_id(entity_id_t id);

	/**
	 * Player ID. Must be unique.
	 */
	player_id_t id;

	/**
	 * Player view of the nyan game data database.
	 */
	std::shared_ptr<nyan::View> db_view;
};

} // namespace openage::gamestate
