// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "game_state.h"

#include "gamestate/game_entity.h"

namespace openage::gamestate {

GameState::GameState(const std::shared_ptr<event::Loop> &event_loop) :
	event::State{event_loop},
	next_id{0} {
}

entity_id_t GameState::get_next_id() {
	auto new_id = this->next_id;
	this->next_id++;

	return new_id;
}

void GameState::add_game_entity(const std::shared_ptr<GameEntity> &entity) {
	if (this->game_entities.contains(entity->get_id())) [[unlikely]] {
		throw Error(MSG(err) << "Game entity with ID " << entity->get_id() << " already exists");
	}
	this->game_entities[entity->get_id()] = entity;
}

} // namespace openage::gamestate
