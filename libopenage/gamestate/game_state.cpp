// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#include "game_state.h"

#include <nyan/nyan.h>

#include "error/error.h"
#include "log/log.h"

#include "gamestate/game_entity.h"
#include "gamestate/player.h"


namespace openage::gamestate {

GameState::GameState(const std::shared_ptr<nyan::Database> &db,
                     const std::shared_ptr<openage::event::EventLoop> &event_loop) :
	event::State{event_loop},
	db_view{db->new_view()} {
}

const std::shared_ptr<nyan::View> &GameState::get_db_view() {
	return this->db_view;
}

void GameState::add_game_entity(const std::shared_ptr<GameEntity> &entity) {
	if (this->game_entities.contains(entity->get_id())) [[unlikely]] {
		throw Error(MSG(err) << "Game entity with ID " << entity->get_id() << " already exists");
	}
	this->game_entities[entity->get_id()] = entity;
}

void GameState::add_player(const std::shared_ptr<Player> &player) {
	if (this->players.contains(player->get_id())) [[unlikely]] {
		throw Error(MSG(err) << "Player with ID " << player->get_id() << " already exists");
	}
	this->players[player->get_id()] = player;
}

void GameState::set_map(const std::shared_ptr<Map> &map) {
	this->map = map;
}

const std::shared_ptr<GameEntity> &GameState::get_game_entity(entity_id_t id) const {
	if (!this->game_entities.contains(id)) [[unlikely]] {
		throw Error(MSG(err) << "Game entity with ID " << id << " does not exist");
	}
	return this->game_entities.at(id);
}

const std::unordered_map<entity_id_t, std::shared_ptr<GameEntity>> &GameState::get_game_entities() const {
	return this->game_entities;
}

const std::shared_ptr<Player> &GameState::get_player(player_id_t id) const {
	if (!this->players.contains(id)) [[unlikely]] {
		throw Error(MSG(err) << "Player with ID " << id << " does not exist");
	}
	return this->players.at(id);
}

const std::shared_ptr<Map> &GameState::get_map() const {
	return this->map;
}

const std::shared_ptr<assets::ModManager> &GameState::get_mod_manager() const {
	return this->mod_manager;
}

void GameState::set_mod_manager(const std::shared_ptr<assets::ModManager> &mod_manager) {
	this->mod_manager = mod_manager;
}

} // namespace openage::gamestate
