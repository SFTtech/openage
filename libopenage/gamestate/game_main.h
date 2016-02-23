// Copyright 2014-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <vector>
#include <SDL2/SDL.h>

#include "player.h"
#include "../options.h"
#include "../terrain/terrain.h"
#include "../unit/unit_container.h"

namespace openage {

class Generator;

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
	 * player by index
	 */
	Player *get_player(unsigned int player_id);

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
	 * no objects should be added of removed once populated
	 */
	std::vector<Player> players;

	/**
	 * all the objects that have been placed.
	 */
	UnitContainer placed_units;

private:

	/**
	 * creates a random civ, owned and managed by this game
	 */
	Civilisation *add_civ(int civ_id);

	/**
	 * civs used in this game
	 */
	std::vector<std::shared_ptr<Civilisation>> civs;

	std::shared_ptr<GameSpec> spec;

};

} // openage
