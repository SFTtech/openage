// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "spawn_entity.h"

#include "coord/phys.h"
#include "gamestate/component/internal/position.h"
#include "gamestate/entity_factory.h"
#include "gamestate/game_entity.h"
#include "gamestate/game_state.h"

namespace openage::gamestate::event {


Spawner::Spawner(const std::shared_ptr<openage::event::EventLoop> &loop) :
	EventEntity(loop) {
}

size_t Spawner::id() const {
	return 0;
}

std::string Spawner::idstr() const {
	return "spawner";
}


SpawnEntityHandler::SpawnEntityHandler(const std::shared_ptr<openage::event::EventLoop> &loop,
                                       const std::shared_ptr<gamestate::EntityFactory> &factory,
                                       const util::Path &animation_path) :
	OnceEventHandler("game.spawn_entity"),
	loop{loop},
	factory{factory},
	animation_path{animation_path} {
}

void SpawnEntityHandler::setup_event(const std::shared_ptr<openage::event::Event> & /* event */,
                                     const std::shared_ptr<openage::event::State> & /* state */) {
	// TODO
}

void SpawnEntityHandler::invoke(openage::event::EventLoop & /* loop */,
                                const std::shared_ptr<openage::event::EventEntity> & /* target */,
                                const std::shared_ptr<openage::event::State> &state,
                                const curve::time_t &time,
                                const param_map &params) {
	auto gstate = std::dynamic_pointer_cast<gamestate::GameState>(state);

	// Create entity
	auto entity = this->factory->add_game_entity(this->animation_path);

	// Setup components
	auto position = std::make_shared<component::Position>(this->loop,
	                                                      params.get("position", coord::phys3{0, 0, 0}),
	                                                      time);
	entity->add_component(static_pointer_cast<component::Component>(position));

	entity->push_to_render();

	gstate->add_game_entity(entity);
}

curve::time_t SpawnEntityHandler::predict_invoke_time(const std::shared_ptr<openage::event::EventEntity> & /* target */,
                                                      const std::shared_ptr<openage::event::State> & /* state */,
                                                      const curve::time_t &at) {
	return at;
}

} // namespace openage::gamestate::event
