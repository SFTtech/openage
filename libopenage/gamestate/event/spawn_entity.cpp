// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "spawn_entity.h"

#include "coord/phys.h"
#include "gamestate/component/internal/activity.h"
#include "gamestate/component/internal/command_queue.h"
#include "gamestate/component/internal/ownership.h"
#include "gamestate/component/internal/position.h"
#include "gamestate/definitions.h"
#include "gamestate/entity_factory.h"
#include "gamestate/game_entity.h"
#include "gamestate/game_state.h"
#include "gamestate/manager.h"

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
                                       const std::shared_ptr<gamestate::EntityFactory> &factory) :
	OnceEventHandler("game.spawn_entity"),
	loop{loop},
	factory{factory} {
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

	auto nyan_db = gstate->get_nyan_db();

	auto game_entities = nyan_db->get_obj_children_all("engine.util.game_entity.GameEntity");

	static uint8_t index = 0;
	static std::vector<nyan::fqon_t> test_entities = {
		"aoe1_base.data.game_entity.generic.chariot_archer.chariot_archer.ChariotArcher",
		"aoe1_base.data.game_entity.generic.bowman.bowman.Bowman",
		"aoe1_base.data.game_entity.generic.hoplite.hoplite.Hoplite",
		"aoe1_base.data.game_entity.generic.temple.temple.Temple",
		"aoe1_base.data.game_entity.generic.academy.academy.Academy",
	};
	nyan::fqon_t nyan_entity = test_entities.at(index);
	++index;
	if (index >= test_entities.size()) {
		index = 0;
	}

	// Create entity
	auto entity = this->factory->add_game_entity(this->loop, gstate, nyan_entity);

	entity->set_manager(std::make_shared<GameEntityManager>(loop, entity));

	// Setup components
	auto entity_pos = std::dynamic_pointer_cast<component::Position>(
		entity->get_component(component::component_t::POSITION));

	auto pos = params.get("position", gamestate::WORLD_ORIGIN);
	entity_pos->set_position(time, pos);
	entity_pos->set_angle(time, coord::phys_angle_t::from_int(315));

	if (entity->has_component(component::component_t::MOVE)) {
		// TODO: Testing values
		entity_pos->set_position(time + 1, pos + coord::phys3_delta{0, 1, 0});
		entity_pos->set_position(time + 2, pos + coord::phys3_delta{1, 2, 0});
		entity_pos->set_position(time + 3, pos + coord::phys3_delta{2, 2, 0});
		entity_pos->set_position(time + 4, pos + coord::phys3_delta{3, 1, 0});
		entity_pos->set_position(time + 5, pos + coord::phys3_delta{3, 0, 0});
		entity_pos->set_position(time + 6, pos + coord::phys3_delta{2, -1, 0});
		entity_pos->set_position(time + 7, pos + coord::phys3_delta{1, -1, 0});
		entity_pos->set_position(time + 8, pos);

		entity_pos->set_angle(time + 1, coord::phys_angle_t::from_int(279));
		entity_pos->set_angle(time + 2, coord::phys_angle_t::from_int(225));
		entity_pos->set_angle(time + 3, coord::phys_angle_t::from_int(180));
		entity_pos->set_angle(time + 4, coord::phys_angle_t::from_int(135));
		entity_pos->set_angle(time + 5, coord::phys_angle_t::from_int(90));
		entity_pos->set_angle(time + 6, coord::phys_angle_t::from_int(45));
		entity_pos->set_angle(time + 7, coord::phys_angle_t::from_int(0));
		entity_pos->set_angle(time + 8, coord::phys_angle_t::from_int(315));
	}

	auto entity_owner = std::dynamic_pointer_cast<component::Ownership>(
		entity->get_component(component::component_t::OWNERSHIP));
	entity_owner->set_owner(time, params.get("owner", 0));

	auto ev = loop->create_event("game.process_command", entity->get_manager(), state, time);
	auto entity_queue = std::dynamic_pointer_cast<component::CommandQueue>(
		entity->get_component(component::component_t::COMMANDQUEUE));
	auto &queue = const_cast<curve::Queue<std::shared_ptr<component::command::Command>> &>(entity_queue->get_queue());
	queue.add_dependent(ev);

	auto activity = std::dynamic_pointer_cast<component::Activity>(
		entity->get_component(component::component_t::ACTIVITY));
	activity->init(time);
	entity->get_manager()->run_activity_system();

	// ASDF: Select the unit when it's created
	// very dumb but it gets the job done
	auto select_cb = params.get("select_cb", std::function<void(entity_id_t id)>{});
	select_cb(entity->get_id());

	entity->push_to_render();

	gstate->add_game_entity(entity);
}

curve::time_t SpawnEntityHandler::predict_invoke_time(const std::shared_ptr<openage::event::EventEntity> & /* target */,
                                                      const std::shared_ptr<openage::event::State> & /* state */,
                                                      const curve::time_t &at) {
	return at;
}

} // namespace openage::gamestate::event
