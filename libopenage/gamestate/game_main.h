// Copyright 2014-2019 the openage authors. See copying.md for legal info.

#pragma once

#include <memory>
#include <vector>
#include <SDL2/SDL.h>

#include <QObject>

#include "market.h"
#include "player.h"
#include "team.h"
#include "../options.h"
#include "../terrain/terrain.h"
#include "../unit/unit_container.h"
#include "../util/timing.h"


namespace openage {

class Engine;
class Generator;
class Terrain;


/**
 * Contains information for a single game
 * This information must be synced across network clients
 *
 * \todo include a list of actions to be saved
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
	 * the number of teams
	 */
	unsigned int team_count() const;

	/**
	 * team by id
	 */
	Team *get_team(unsigned int team_id);

	/**
	 * the spec in this games settings
	 */
	GameSpec *get_spec();

	/**
	 * updates the game by one frame
	 */
	void update(time_nsec_t lastframe_duration);

	/**
	 * map information
	 */
	std::shared_ptr<Terrain> terrain;

	/**
	 * all teams in the game
	 */
	std::vector<Team> teams;

	/**
	 * The global market (the global market prices).
	 */
	Market market;

	/**
	 * all the objects that have been placed.
	 */
	UnitContainer placed_units;

private:

	/**
	 * all players in the game
	 * no objects should be added of removed once populated
	 */
	std::vector<std::shared_ptr<Player>> players;

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

namespace qtsdl {
class GuiItemLink;
} // qtsdl

namespace openage {

class GameMainSignals : public QObject {
	Q_OBJECT

public:
signals:
	void game_running(bool running);
};


/**
 * Class linked to the QML object "GameMain" via GameMainLink.
 * Gets instanciated from QML.
 */
class GameMainHandle {
public:
	explicit GameMainHandle(qtsdl::GuiItemLink *gui_link);

	void set_engine(Engine *engine);

	/**
	 * End the game and delete the game handle.
	 */
	void clear();

	/**
	 * Pass the given game to the engine and start it.
	 */
	void set_game(std::unique_ptr<GameMain> &&game);

	/**
	 * Return the game.
	 */
	GameMain* get_game() const;

	/**
	 * Test if there is a game running.
	 */
	bool is_game_running() const;

	/**
	 * Emit a qt signal to notify for changes in a running game.
	 */
	void announce_running();

private:
	/**
	 * The game state as currently owned by the engine,
	 * just remembered here to access it quickly.
	 */
	GameMain *game;

	/**
	 * The engine the main game handle is attached to.
	 */
	Engine *engine;

public:
	GameMainSignals gui_signals;
	qtsdl::GuiItemLink *gui_link;
};

} // openage
