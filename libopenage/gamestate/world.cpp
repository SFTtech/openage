// Copyright 2018-2018 the openage authors. See copying.md for legal info.

#include "world.h"

#include "gamestate/game_entity.h"
#include "renderer/entity_factory.h"


namespace openage::gamestate {

World::World(const util::Path &root_dir) :
	game_entities{} {
	// ASDF: testing
	auto tex0_path = root_dir / "assets" / "gaben.png";
	auto pos0 = util::Vector3f(3.0f, 3.0f, 0.0f);
	auto ge0 = std::make_shared<GameEntity>(1, pos0, tex0_path);
	this->game_entities.push_back(ge0);

	auto tex1_path = root_dir / "assets" / "missing.png";
	auto pos1 = util::Vector3f(7.5f, 6.0f, 0.0f);
	auto ge1 = std::make_shared<GameEntity>(2, pos1, tex1_path);
	this->game_entities.push_back(ge1);
}

void World::attach_renderer(const std::shared_ptr<renderer::RenderFactory> &render_factory) {
	this->render_factory = render_factory;

	// ASDF: testing
	for (auto game_entity : game_entities) {
		auto world_render_entity = this->render_factory->add_world_render_entity();
		game_entity->set_render_entity(world_render_entity);
	}
}


} // namespace openage::gamestate
