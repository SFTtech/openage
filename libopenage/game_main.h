// Copyright 2014-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <vector>
#include <SDL2/SDL.h>

#include "engine.h"
#include "options.h"
#include "player.h"
#include "terrain/terrain.h"
#include "unit/unit_container.h"

namespace openage {

/**
 * Contains information for a single game
 * This information must be synced across network clients
 *
 * TODO: include a list of actions to be saved
 *       as the game replay file
 */
class GameMain : public options::OptionNode {
public:
	GameMain(const Generator &generator);
	~GameMain();

	/**
	 * the number of players
	 */
	unsigned int player_count() const;

	/**
	 * the spec in this games settings
	 */
	GameSpec *get_spec();

	/**
	 * updates the game by one frame
	 */
	void update();

	/**
	 * map information
	 */
	std::shared_ptr<Terrain> terrain;

	/**
	 * all players in the game
	 */
	std::vector<Player> players;

	/**
	 * all the objects that have been placed.
	 */
	UnitContainer placed_units;

private:
	std::shared_ptr<GameSpec> spec;

};

} // openage
