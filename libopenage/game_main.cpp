// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "unit/unit_type.h"
#include "game_main.h"
#include "game_spec.h"
#include "generator.h"

namespace openage {

GameMain::GameMain(const Generator &generator)
	:
	OptionNode{"GameMain"},
	terrain{generator.terrain()},
	spec{generator.get_spec()} {

	// players
	unsigned int i = 0;
	for (auto &name : generator.player_names()) {
		this->players.emplace_back(i, name, i);
		i++;
	}
	for (auto &player : this->players) {
		player.initialise_unit_types(*this->spec);
	}

	this->placed_units.set_terrain(this->terrain);
	generator.add_units(*this);
}

GameMain::~GameMain() {
	log::log(MSG(warn) << "Cleanup gamemain");
}

unsigned int GameMain::player_count() const {
	return this->players.size();
}

Player *GameMain::get_player(unsigned int player_id) {
	return &this->players.at(player_id);
}

GameSpec *GameMain::get_spec() {
	return this->spec.get();
}

void GameMain::update() {
	this->placed_units.update_all();
}

} // openage
