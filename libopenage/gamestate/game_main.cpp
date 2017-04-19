// Copyright 2014-2017 the openage authors. See copying.md for legal info.

#include "game_main.h"

#include "../engine.h"
#include "../log/log.h"
#include "../scripting/script_singleton.h"
#include "../terrain/terrain.h"
#include "../unit/unit_type.h"
#include "game_spec.h"
#include "generator.h"


namespace openage {

GameMain::GameMain(const Generator &generator)
	:
	OptionNode{"GameMain"},
	terrain{generator.terrain()},
	placed_units{},
	spec{generator.get_spec()} {

	// players
	unsigned int i = 0;
	for (auto &name : generator.player_names()) {
		this->players.emplace_back(this->add_civ(i), i, name);
		i++;
	}

	// initialise types only after all players are added
	for (auto &p : this->players) {
		p.initialise_unit_types();
	}

	// initialise units
	this->placed_units.set_terrain(this->terrain);
	generator.add_units(*this);
    
    // initialise scripts
    // must be the last one to initialise of game, because scripts can access
    // and modify gamestate from there
    // its expermintal/proof of concept so we use singleton
    ScriptSingleton::instance().addScript(new Script("libopenage/scripting/openage.lua", this));
    this->gametime=0;
}

GameMain::~GameMain() {
    for(auto script : ScriptSingleton::instance().scripts) {
        delete script;
    }
    ScriptSingleton::instance().scripts.clear();
}

unsigned int GameMain::player_count() const {
	return this->players.size();
}

Player *GameMain::get_player(unsigned int player_id) {
	return &this->players.at(player_id);
}

unsigned int GameMain::team_count() const {
	return this->teams.size();
}

Team *GameMain::get_team(unsigned int team_id) {
	return &this->teams.at(team_id);
}

GameSpec *GameMain::get_spec() {
	return this->spec.get();
}

void GameMain::update(time_nsec_t lastframe_duration) {
	this->placed_units.update_all(lastframe_duration);

    // update lua scripts
    this->gametime += lastframe_duration;
    for(auto script : ScriptSingleton::instance().scripts) {
        script->update(lastframe_duration, this->gametime);
    }
}

Civilisation *GameMain::add_civ(int civ_id) {
	auto new_civ = std::make_shared<Civilisation>(*this->spec, civ_id);
	this->civs.emplace_back(new_civ);
	return new_civ.get();
}

GameMainHandle::GameMainHandle(qtsdl::GuiItemLink *gui_link)
	:
	game{},
	engine{},
	gui_link{gui_link} {
}

void GameMainHandle::set_engine(Engine *engine) {
	ENSURE(!this->engine || this->engine == engine, "relinking GameMain to another engine is not supported and not caught properly");
	this->engine = engine;
}

void GameMainHandle::clear() {
	if (this->engine) {
		this->game = nullptr;
		this->engine->end_game();
		announce_running();
	}
}

void GameMainHandle::set_game(std::unique_ptr<GameMain> &&game) {
	if (this->engine) {
		ENSURE(game, "linking game to engine problem");

		// remember the pointer
		this->game = game.get();

		// then pass on the game to the engine
		this->engine->start_game(std::move(game));

		announce_running();
	}
}

GameMain *GameMainHandle::get_game() const {
	return this->game;
}

bool GameMainHandle::is_game_running() const {
	return this->game != nullptr;
}

void GameMainHandle::announce_running() {
	emit this->gui_signals.game_running(this->game);
}

} // openage
