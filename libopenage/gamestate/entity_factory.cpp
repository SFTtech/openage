// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "entity_factory.h"

#include "gamestate/game_entity.h"
#include "gamestate/game_state.h"
#include "renderer/render_factory.h"

namespace openage::gamestate {

EntityFactory::EntityFactory(const std::shared_ptr<GameState> &state) :
	state{state},
	render_factory{nullptr} {
}

std::shared_ptr<GameEntity> EntityFactory::add_game_entity(util::Vector3f pos,
                                                           util::Path &texture_path) {
	auto entity = std::make_shared<GameEntity>(this->state->get_next_id(),
	                                           pos,
	                                           texture_path);
	this->state->add_game_entity(entity);

	if (this->render_factory) {
		entity->set_render_entity(this->render_factory->add_world_render_entity());
	}

	return entity;
}

void EntityFactory::attach_renderer(const std::shared_ptr<renderer::RenderFactory> &render_factory) {
	this->render_factory = render_factory;
}

} // namespace openage::gamestate
