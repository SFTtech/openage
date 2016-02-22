// Copyright 2014-2016 the openage authors. See copying.md for legal info.

#include "../unit/unit_type.h"
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
}

GameMain::~GameMain() {
	log::log(MSG(info) << "Cleanup gamemain");
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

Civilisation *GameMain::add_civ(int civ_id) {
	auto new_civ = std::make_shared<Civilisation>(*this->spec, civ_id);
	this->civs.emplace_back(new_civ);
	return new_civ.get();
}

} // openage
