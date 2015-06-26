// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_GAME_MAIN_H_
#define OPENAGE_GAME_MAIN_H_

#include <memory>
#include <vector>
#include <SDL2/SDL.h>

#include "engine.h"
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
class GameMain {
public:
	GameMain(const game_settings &sets);
	~GameMain();

	/**
	 * get the game settings
	 */
	game_settings *get_settings();

	/**
	 * the spec in this games settings
	 */
	GameSpec *get_spec();

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
	game_settings settings;

};

} //namespace openage

#endif
