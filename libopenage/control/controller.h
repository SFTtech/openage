// Copyright 2021-2021 the openage authors. See copying.md for legal info.

#pragma once

#include <queue>
#include <stddef.h>
#include <unordered_set>

#include "../curve/discrete.h"

namespace openage::control {

/**
 * Interface for game controllers.
 *
 * Controllers handle inputs from outside of a game (e.g. GUI, AI, scripts, ...)
 * and pass the resulting events to game entities. They also act as a form of
 * access control for using in-game functionality of game entities.
 */
class Controller {
public:
	Controller(/*Engine *engine,*/
	           /*GameMainHandle *game,*/
	           const std::unordered_set<size_t> controlled_factions,
	           size_t active_faction_id);

	// void set_engine(Engine *engine);
	// void set_game(GameMainHandle *game);

	/**
     * Switch the actively controlled faction by the controller.
     * The ID must be in the list of controlled factions.
     *
     * @param faction_id ID of the new active faction.
     */
	void set_active_faction_id(size_t faction_id);

	// Engine *get_engine() const;
	// GameMainHandle *get_game() const;
	// Player *get_active_faction() const;

	/**
     * Get the ID of the faction actively controlled by the controller.
     *
     * @return ID of the active faction.
     */
	const size_t get_active_faction_id() const;

private:
	// Engine *engine;
	// GameMainHandle *game;

	/**
     * List of factions controllable by this controller.
     */
	std::unordered_set<size_t> controlled_factions;

	/**
     * ID of the currently active faction.
     */
	size_t active_faction_id;

	/**
     * IDs of the currently controlled game objects (aka selection queue).
     */
	std::vector<size_t> control_queue;
};

} // namespace openage::control