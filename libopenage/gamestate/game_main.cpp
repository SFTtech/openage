// Copyright 2014-2016 the openage authors. See copying.md for legal info.

#include "../engine.h"
#include "../terrain/terrain.h"
#include "../unit/unit_type.h"
#include "game_main.h"

#include "../curve/curve_record_replay.h"
#include "../unit/attribute_watcher.h"
#include "game_spec.h"
#include "generator.h"

namespace openage {

GameMain::GameMain(const Generator &generator)
	:
	OptionNode{"GameMain"},
	terrain{generator.terrain()},
	spec{generator.get_spec()},
	record_replay{} {

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

	curve::CurveRecord watcher{nullptr};
	generator.add_units(*this, watcher);
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
	if (this->record_replay) {
		this->record_replay->perform(this->placed_units);
	} else {
		// TODO: store pointer to the printer as a member, make bindable
		curve::CurveRecord watcher{nullptr};
		this->placed_units.update_all(watcher);
	}
}

curve::CurveRecordReplay* GameMain::get_record_replay() const {
	return this->record_replay;
}

curve::CurveRecord* GameMain::get_record() const {
	return this->record_replay ? this->record_replay->get_record() : nullptr;
}

void GameMain::set_record_replay(curve::CurveRecordReplay *record_replay) {
	this->record_replay = record_replay;
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
	// TODO: decide to either go for a full Engine QML-singleton or for a regular object
	ENSURE(!this->engine || this->engine == engine, "relinking GameMain to another engine is not supported and not caught properly");
	this->engine = engine;
}

curve::CurveRecordReplay* GameMainHandle::get_record_replay() const {
	return this->game ? this->game->get_record_replay() : nullptr;
}

void GameMainHandle::set_record_replay(curve::CurveRecordReplay *record_replay) {
	if (this->game)
		this->game->set_record_replay(record_replay);
}

void GameMainHandle::clear() {
	if (this->engine) {
		this->game = nullptr;
		this->engine->end_game();
		announce_running();
	}
}

void GameMainHandle::set_game(std::unique_ptr<GameMain> game) {
	if (this->engine) {
		ENSURE(game, "linking game to engine problem");
		this->game = game.get();
		this->engine->start_game(std::move(game));
		announce_running();
	}
}

GameMain* GameMainHandle::get_game() const {
	return this->game;
}

bool GameMainHandle::is_game_running() const {
	return this->game;
}

void GameMainHandle::announce_running() {
	emit this->gui_signals.game_running(this->game);
}

} // openage
