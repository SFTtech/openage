// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "entity_factory.h"

#include "event/state.h"
#include "gamestate/component/api/idle.h"
#include "gamestate/component/api/live.h"
#include "gamestate/component/api/move.h"
#include "gamestate/component/api/turn.h"
#include "gamestate/component/internal/command_queue.h"
#include "gamestate/component/internal/ownership.h"
#include "gamestate/component/internal/position.h"
#include "gamestate/game_entity.h"
#include "gamestate/game_state.h"
#include "renderer/render_factory.h"

namespace openage::gamestate {

EntityFactory::EntityFactory() :
	next_id{0},
	render_factory{nullptr} {
}

std::shared_ptr<GameEntity> EntityFactory::add_game_entity(util::Path &animation_path) {
	std::unique_lock lock{this->mutex};

	auto entity = std::make_shared<GameEntity>(this->get_next_id(),
	                                           animation_path);

	if (this->render_factory) {
		entity->set_render_entity(this->render_factory->add_world_render_entity());
	}

	return entity;
}

std::shared_ptr<GameEntity> EntityFactory::add_game_entity(const std::shared_ptr<openage::event::EventLoop> &loop,
                                                           const std::shared_ptr<GameState> &state,
                                                           const nyan::fqon_t &nyan_entity) {
	auto entity = std::make_shared<GameEntity>(this->get_next_id());

	initialize_components(loop, state, entity, nyan_entity);

	return entity;
}

void EntityFactory::attach_renderer(const std::shared_ptr<renderer::RenderFactory> &render_factory) {
	std::unique_lock lock{this->mutex};

	this->render_factory = render_factory;
}

void EntityFactory::initialize_components(const std::shared_ptr<openage::event::EventLoop> &loop,
                                          const std::shared_ptr<GameState> &state,
                                          const std::shared_ptr<GameEntity> &entity,
                                          const nyan::fqon_t &nyan_entity) {
	auto position = std::make_shared<component::Position>(loop);
	entity->add_component(position);

	auto ownership = std::make_shared<component::Ownership>(loop);
	entity->add_component(ownership);

	auto command_queue = std::make_shared<component::CommandQueue>(loop);
	entity->add_component(command_queue);

	auto db_view = state->get_nyan_db();
	auto nyan_obj = db_view->get_object(nyan_entity);
	auto abilities = nyan_obj.get_set("abilities");
	for (auto &ability_val : abilities) {
		auto ability_fqon = std::dynamic_pointer_cast<nyan::ObjectValue>(ability_val.get_ptr())->get_name();
		auto ability_obj = db_view->get_object(ability_fqon);

		auto ability_parent = ability_obj.get_parents()[0];
		if (ability_parent == "engine.ability.type.Move") {
			auto move = std::make_shared<component::Move>(loop, ability_obj);
			entity->add_component(move);
		}
		else if (ability_parent == "engine.ability.type.Turn") {
			auto turn = std::make_shared<component::Turn>(loop, ability_obj);
			entity->add_component(turn);
		}
		else if (ability_parent == "engine.ability.type.Idle") {
			auto idle = std::make_shared<component::Idle>(loop, ability_obj);
			entity->add_component(idle);
		}
		else if (ability_parent == "engine.ability.type.Live") {
			auto live = std::make_shared<component::Live>(loop, ability_obj);
			entity->add_component(live);
		}
	}
}

entity_id_t EntityFactory::get_next_id() {
	auto new_id = this->next_id;
	this->next_id++;

	return new_id;
}

} // namespace openage::gamestate
