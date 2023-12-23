// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "entity_factory.h"

#include <cstdint>
#include <deque>
#include <mutex>
#include <string>
#include <unordered_set>

#include "error/error.h"

#include "curve/discrete.h"
#include "curve/queue.h"
#include "event/event_loop.h"
#include "gamestate/activity/activity.h"
#include "gamestate/activity/end_node.h"
#include "gamestate/activity/start_node.h"
#include "gamestate/activity/task_system_node.h"
#include "gamestate/activity/xor_event_gate.h"
#include "gamestate/activity/xor_gate.h"
#include "gamestate/api/activity.h"
#include "gamestate/component/api/idle.h"
#include "gamestate/component/api/live.h"
#include "gamestate/component/api/move.h"
#include "gamestate/component/api/turn.h"
#include "gamestate/component/internal/activity.h"
#include "gamestate/component/internal/command_queue.h"
#include "gamestate/component/internal/ownership.h"
#include "gamestate/component/internal/position.h"
#include "gamestate/component/types.h"
#include "gamestate/game_entity.h"
#include "gamestate/game_state.h"
#include "gamestate/manager.h"
#include "gamestate/player.h"
#include "gamestate/system/types.h"
#include "log/message.h"
#include "renderer/render_factory.h"
#include "time/time.h"
#include "util/fixed_point.h"

#include "gamestate/event/process_command.h"
#include "gamestate/event/wait.h"

namespace openage::gamestate {

/**
 * Create a simple test activity for the game entity.
 *
 * The activity is as follows:
 *                      |------------------------------------------------------|
 *                      |                                                      v
 * Start -> Idle -> Condition -> Wait for command -> Move -> Wait for move -> End
 *            ^                                                      |
 *            |------------------------------------------------------|
 *
 * TODO: Should be:
 * 			               |----------------------------------------------------------------------|
 * 			    	       |                                                                      v
 * Start -> Idle -> -> Condition -> Wait for command <-> Condition -> Move -> Wait or command -> End
 * 		      ^                                             |^                     |
 * 		      |---------------------------------------------||---------------------|
 * (new condition in the middle: check if there is a command, if not go back to wait for command)
 * (new node: go back to node 1 if there is no command)
 * (node 5: wait for a command OR for a the wait time)
 *
 * TODO: Replace with config
 */
std::shared_ptr<activity::Activity> create_test_activity() {
	auto start = std::make_shared<activity::StartNode>(0);
	auto idle = std::make_shared<activity::TaskSystemNode>(1, "Idle");
	auto condition_moveable = std::make_shared<activity::XorGate>(2);
	auto wait_for_command = std::make_shared<activity::XorEventGate>(3);
	auto condition_command = std::make_shared<activity::XorGate>(4);
	auto move = std::make_shared<activity::TaskSystemNode>(5, "Move");
	auto wait_for_move = std::make_shared<activity::XorEventGate>(6);
	auto end = std::make_shared<activity::EndNode>(7);

	start->add_output(idle);

	idle->add_output(condition_moveable);
	idle->set_system_id(system::system_id_t::IDLE);

	// wait_for_command branch
	activity::condition_t wait_branch = [&](const time::time_t & /* time */,
	                                        const std::shared_ptr<gamestate::GameEntity> &entity) {
		return entity->has_component(component::component_t::MOVE);
	};
	condition_moveable->add_output(wait_for_command, wait_branch);

	// end branch
	condition_moveable->set_default(end);

	wait_for_command->add_output(move, gamestate::event::primer_process_command);

	move->add_output(wait_for_move);
	move->set_system_id(system::system_id_t::MOVE_COMMAND);

	wait_for_move->add_output(idle, gamestate::event::primer_wait);
	wait_for_move->add_output(move, gamestate::event::primer_process_command);

	return std::make_shared<activity::Activity>(0, start, "test");
}

EntityFactory::EntityFactory() :
	next_entity_id{0},
	next_player_id{0},
	render_factory{nullptr} {
}

std::shared_ptr<GameEntity> EntityFactory::add_game_entity(const std::shared_ptr<openage::event::EventLoop> &loop,
                                                           const std::shared_ptr<GameState> &state,
                                                           player_id_t owner_id,
                                                           const nyan::fqon_t &nyan_entity) {
	auto entity = std::make_shared<GameEntity>(this->get_next_entity_id());
	entity->set_manager(std::make_shared<GameEntityManager>(loop, state, entity));

	// use the owner's data to initialize the entity
	// this ensures that only the owner's tech upgrades apply
	auto db_view = state->get_player(owner_id)->get_db_view();
	init_components(loop, db_view, entity, nyan_entity);

	if (this->render_factory) {
		entity->set_render_entity(this->render_factory->add_world_render_entity());
	}

	return entity;
}

std::shared_ptr<Player> EntityFactory::add_player(const std::shared_ptr<openage::event::EventLoop> & /* loop */,
                                                  const std::shared_ptr<GameState> &state,
                                                  const nyan::fqon_t & /* player_setup */) {
	auto player = std::make_shared<Player>(this->get_next_player_id(),
	                                       state->get_db_view()->new_child());

	// TODO: Components (for resources, diplomacy, etc.)

	return player;
}

void EntityFactory::attach_renderer(const std::shared_ptr<renderer::RenderFactory> &render_factory) {
	std::unique_lock lock{this->mutex};

	this->render_factory = render_factory;
}

void EntityFactory::init_components(const std::shared_ptr<openage::event::EventLoop> &loop,
                                    const std::shared_ptr<nyan::View> &owner_db_view,
                                    const std::shared_ptr<GameEntity> &entity,
                                    const nyan::fqon_t &nyan_entity) {
	auto position = std::make_shared<component::Position>(loop);
	entity->add_component(position);

	auto ownership = std::make_shared<component::Ownership>(loop);
	entity->add_component(ownership);

	auto command_queue = std::make_shared<component::CommandQueue>(loop);
	entity->add_component(command_queue);

	auto nyan_obj = owner_db_view->get_object(nyan_entity);
	nyan::set_t abilities = nyan_obj.get_set("GameEntity.abilities");

	std::optional<nyan::Object> activity_ability;
	for (const auto &ability_val : abilities) {
		auto ability_fqon = std::dynamic_pointer_cast<nyan::ObjectValue>(ability_val.get_ptr())->get_name();
		auto ability_obj = owner_db_view->get_object(ability_fqon);

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

			auto attr_settings = ability_obj.get_set("Live.attributes");
			for (auto &setting : attr_settings) {
				auto setting_obj_val = std::dynamic_pointer_cast<nyan::ObjectValue>(setting.get_ptr());
				auto setting_obj = owner_db_view->get_object(setting_obj_val->get_name());
				auto attribute = setting_obj.get_object("AttributeSetting.attribute");
				auto start_value = setting_obj.get_int("AttributeSetting.starting_value");

				live->add_attribute(std::numeric_limits<time::time_t>::min(),
				                    attribute.get_name(),
				                    std::make_shared<curve::Discrete<int64_t>>(loop,
				                                                               0,
				                                                               "",
				                                                               nullptr,
				                                                               start_value));
			}
		}
		else if (ability_parent == "engine.ability.type.Activity") {
			activity_ability = ability_obj;
		}
	}

	// if (activity_ability) {
	// 	init_activity(loop, owner_db_view, entity, activity_ability.value());
	// }
	// else {
	auto activity = std::make_shared<component::Activity>(loop, create_test_activity());
	entity->add_component(activity);
	// }
}

void EntityFactory::init_activity(const std::shared_ptr<openage::event::EventLoop> &loop,
                                  const std::shared_ptr<nyan::View> &owner_db_view,
                                  const std::shared_ptr<GameEntity> &entity,
                                  const nyan::Object &ability) {
	nyan::Object graph = ability.get_object("Activity.graph");
	auto start_obj = api::APIActivity::get_start(graph);

	size_t node_id = 0;

	std::deque<nyan::Object> nyan_nodes;
	std::unordered_map<size_t, std::shared_ptr<activity::Node>> node_id_map{};
	std::unordered_map<nyan::fqon_t, size_t> visited{};
	std::shared_ptr<activity::Node> start_node;

	// First pass: create all nodes using breadth-first search
	nyan_nodes.push_back(start_obj);
	while (!nyan_nodes.empty()) {
		auto node = nyan_nodes.front();
		nyan_nodes.pop_front();

		if (visited.contains(node.get_name())) {
			continue;
		}

		// Create the node
		switch (api::APIActivityNode::get_type(node)) {
		case activity::node_t::END:
			break;
		case activity::node_t::START:
			start_node = std::make_shared<activity::StartNode>(node_id);
			node_id_map[node_id] = start_node;
			break;
		case activity::node_t::TASK_SYSTEM:
			node_id_map[node_id] = std::make_shared<activity::TaskSystemNode>(node_id);
			// TODO: Set system ID
			break;
		case activity::node_t::XOR_GATE:
			node_id_map[node_id] = std::make_shared<activity::XorGate>(node_id);
			break;
		case activity::node_t::XOR_EVENT_GATE:
			node_id_map[node_id] = std::make_shared<activity::XorEventGate>(node_id);
			break;
		default:
			throw Error{ERR << "Unknown activity node type"};
		}

		// Get the node's outputs
		auto next_nodes = api::APIActivityNode::get_next(node);
		nyan_nodes.insert(nyan_nodes.end(), next_nodes.begin(), next_nodes.end());

		visited.insert({node.get_name(), node_id});
		node_id++;
	}

	// Second pass: connect the nodes
	for (const auto &current_node : visited) {
		auto nyan_node = owner_db_view->get_object(current_node.first);
		auto activity_node = node_id_map[current_node.second];

		auto next_nodes = api::APIActivityNode::get_next(nyan_node);
		for (const auto &next_node : next_nodes) {
			auto next_node_id = visited[next_node.get_name()];
			auto next_engine_node = node_id_map[next_node_id];

			switch (activity_node->get_type()) {
			case activity::node_t::END:
				break;
			case activity::node_t::START: {
				auto start = std::static_pointer_cast<activity::StartNode>(activity_node);
				start->add_output(next_engine_node);
				break;
			}
			case activity::node_t::TASK_SYSTEM: {
				auto task_system = std::static_pointer_cast<activity::TaskSystemNode>(activity_node);
				task_system->add_output(next_engine_node);
				break;
			}
			case activity::node_t::XOR_GATE: {
				auto xor_gate = std::static_pointer_cast<activity::XorGate>(activity_node);
				// TODO: Add conditions
				break;
			}
			case activity::node_t::XOR_EVENT_GATE: {
				auto xor_event_gate = std::static_pointer_cast<activity::XorEventGate>(activity_node);
				xor_event_gate->add_output(next_engine_node, api::APIActivityEvent::get_primer(next_node));
				break;
			}
			default:
				throw Error{ERR << "Unknown activity node type"};
			}
		}
	}

	auto activity = std::make_shared<activity::Activity>(0, start_node, graph.get_name());

	auto component = std::make_shared<component::Activity>(loop, activity);
	entity->add_component(component);
}

entity_id_t EntityFactory::get_next_entity_id() {
	auto new_id = this->next_entity_id;
	this->next_entity_id++;

	return new_id;
}

player_id_t EntityFactory::get_next_player_id() {
	auto new_id = this->next_player_id;
	this->next_player_id++;

	return new_id;
}

} // namespace openage::gamestate
