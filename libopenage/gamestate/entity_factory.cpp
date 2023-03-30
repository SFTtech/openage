// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "entity_factory.h"

#include "gamestate/game_entity.h"
#include "gamestate/game_state.h"

namespace openage::gamestate {

EntityFactory::EntityFactory(const std::shared_ptr<GameState> &state) :
	state{state} {
}

std::shared_ptr<GameEntity> EntityFactory::add_game_entity(util::Vector3f pos,
                                                           util::Path &texture_path) {
	auto entity = std::make_shared<GameEntity>(this->state->get_next_id(),
	                                           pos,
	                                           texture_path);
	this->state->add_game_entity(entity);

	return entity;
}

} // namespace openage::gamestate
