// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#include "game_main.h"
#include "game_spec.h"

namespace openage {

// size of the default terrain
constexpr coord::tile_delta terrain_data_size = {16, 16};

// terrain ids for the default terrain
constexpr int terrain_data[16 * 16] = {
	  0,  0,  0,  0,  0,  0,  0,  0, 16,  0,  2,  1, 15, 15, 15,  1,
	  0, 18, 18, 18, 18, 18,  0,  0, 16,  0,  2,  1, 15, 14, 15,  1,
	 18, 18,  0,  0, 18, 18,  0,  0, 16,  0,  2,  1, 15, 15, 15,  1,
	 18, 18,  0,  0, 18, 18,  0,  0, 16,  0,  2,  1,  1,  1,  2,  2,
	 18, 18, 18,  0, 18, 18,  9,  9, 16,  0,  0,  2,  2,  2,  0,  0,
	 18, 18,  0,  0,  0,  0,  9,  9, 16,  0,  0,  0,  0,  0,  0,  0,
	  0, 18,  0,  0,  0,  9,  9,  9, 16,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  2,  0,  9,  9,  0,  2,  2,  0,  0,  0,  0, 23, 23,
	  0,  0,  2, 15,  2,  9,  0,  2, 15, 15,  2,  0,  0,  0,  0,  0,
	  0,  0,  2, 15,  2,  2,  2, 15,  2,  2,  0,  0,  0,  0,  0,  0,
	  0,  0,  2, 15,  2,  2,  2, 15,  2,  0,  0,  0, 20, 20, 20,  0,
	  0,  2,  2, 15,  2,  2,  2, 14,  2,  0,  0,  0, 21, 21, 21,  0,
	  2, 15, 15, 15, 15, 15, 14, 14,  2,  0,  0,  0, 22, 22, 22,  0,
	  0,  2,  2,  2,  2,  2,  2,  2,  0,  0,  0,  0,  5,  5,  5,  0,
	  0,  0,  0,  0,  0,  0,  0,  0, 16, 16, 16, 16,  5,  5,  5,  5,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  5,  5
};

GameMain::GameMain(const game_settings &sets)
	:
	terrain{std::make_shared<Terrain>(sets.spec->get_terrain_meta(), true)},
	settings(sets) {

	// the initial map should be determined by game settings
	this->terrain->fill(terrain_data, terrain_data_size);
	this->placed_units.set_terrain(this->terrain);

	// players
	for (unsigned int i = 0; i < this->settings.number_of_players; ++i) {
		this->players.emplace_back(i);
	}

}

GameMain::~GameMain() {}

game_settings *GameMain::get_settings() {
	return &this->settings;
}

GameSpec *GameMain::get_spec() {
	return this->settings.spec.get();
}

void GameMain::update() {
	this->placed_units.update_all();
}

} //namespace openage
