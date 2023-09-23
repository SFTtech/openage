// Copyright 2014-2023 the openage authors. See copying.md for legal info.

#include "game_main.h"

#include "../../log/log.h"
#include "../../terrain/terrain.h"
#include "../../unit/unit_type.h"
#include "game_spec.h"
#include "generator.h"


namespace openage {

GameMain::GameMain(const Generator &generator) :
	OptionNode{"GameMain"},
	terrain{generator.terrain()},
	placed_units{},
	spec{generator.get_spec()} {
	// players
	this->players.reserve(generator.player_names().size());
	unsigned int i = 0;
	for (auto &name : generator.player_names()) {
		this->players.push_back(std::make_shared<Player>(this->add_civ(i), i, name));
		i++;
	}

	// initialise types only after all players are added
	for (auto &p : this->players) {
		p->initialise_unit_types();
	}

	// initialise units
	this->placed_units.set_terrain(this->terrain);
	generator.add_units(*this);
}

GameMain::~GameMain() = default;

unsigned int GameMain::player_count() const {
	return this->players.size();
}

Player *GameMain::get_player(unsigned int player_id) {
	return this->players.at(player_id).get();
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
}

Civilisation *GameMain::add_civ(int civ_id) {
	auto new_civ = std::make_shared<Civilisation>(*this->spec, civ_id);
	this->civs.emplace_back(new_civ);
	return new_civ.get();
}

GameMainHandle::GameMainHandle(qtsdl::GuiItemLink *gui_link) :
	game{},
	gui_link{gui_link} {
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

} // namespace openage
