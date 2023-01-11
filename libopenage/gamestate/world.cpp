// Copyright 2018-2018 the openage authors. See copying.md for legal info.

#include "world.h"

#include "gamestate/game_entity.h"
#include "renderer/entity_factory.h"


namespace openage::gamestate {

World::World(const util::Path &root_dir) :
	game_entities{} {
	// TODO
}

void World::attach_renderer(const std::shared_ptr<renderer::RenderFactory> &render_factory) {
	this->render_factory = render_factory;

	for (auto game_entity : game_entities) {
		auto world_render_entity = this->render_factory->add_world_render_entity();
		game_entity->set_render_entity(world_render_entity);
	}
}


} // namespace openage::gamestate
