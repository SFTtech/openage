// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#include "activity.h"

#include <functional>
#include <string>
#include <vector>

#include "error/error.h"
#include "log/message.h"

#include "gamestate/activity/node.h"
#include "gamestate/activity/start_node.h"
#include "gamestate/activity/task_node.h"
#include "gamestate/activity/task_system_node.h"
#include "gamestate/activity/types.h"
#include "gamestate/activity/xor_event_gate.h"
#include "gamestate/activity/xor_gate.h"
#include "gamestate/component/internal/activity.h"
#include "gamestate/component/types.h"
#include "gamestate/game_entity.h"
#include "gamestate/system/idle.h"
#include "gamestate/system/move.h"
#include "util/fixed_point.h"


namespace openage::gamestate::system {


void Activity::advance(const time::time_t &start_time,
                       const std::shared_ptr<gamestate::GameEntity> &entity,
                       const std::shared_ptr<openage::event::EventLoop> &loop,
                       const std::shared_ptr<openage::gamestate::GameState> &state,
                       const std::optional<openage::event::EventHandler::param_map> &ev_params) {
	auto activity_component = std::dynamic_pointer_cast<component::Activity>(
		entity->get_component(component::component_t::ACTIVITY));
	auto current_node = activity_component->get_node(start_time);

	if (current_node == nullptr) [[unlikely]] {
		throw Error{ERR << "No node defined in activity graph for entity "
		                << std::to_string(entity->get_id()) << " (t=" << start_time << ")"};
	}

	// TODO: this check should be moved to a more general pre-processing section
	if (current_node->get_type() == activity::node_t::XOR_EVENT_GATE) {
		// returning to a event gateway means that the event has been triggered
		// move to the next node here
		if (not ev_params.has_value()) {
			throw Error{ERR << "XorEventGate: No event parameters given on continue"};
		}

		auto next_id = ev_params.value().get<size_t>("next");
		current_node = current_node->next(next_id);

		// cancel all other events that the manager may have been waiting for
		activity_component->cancel_events(start_time);
	}

	time::time_t event_wait_time = 0;
	auto stop = false;
	while (not stop) {
		switch (current_node->get_type()) {
		case activity::node_t::START: {
			auto node = std::static_pointer_cast<activity::StartNode>(current_node);
			auto next_id = node->get_next();
			current_node = node->next(next_id);
		} break;
		case activity::node_t::END: {
			// TODO: if activities are nested, advance to parent activity
			stop = true;
		} break;
		case activity::node_t::TASK_CUSTOM: {
			auto node = std::static_pointer_cast<activity::TaskCustom>(current_node);
			auto task = node->get_task_func();
			task(start_time, entity);
			auto next_id = node->get_next();
			current_node = node->next(next_id);
		} break;
		case activity::node_t::TASK_SYSTEM: {
			auto node = std::static_pointer_cast<activity::TaskSystemNode>(current_node);
			auto task = node->get_system_id();
			event_wait_time = Activity::handle_subsystem(start_time, entity, state, task);
			auto next_id = node->get_next();
			current_node = node->next(next_id);
		} break;
		case activity::node_t::XOR_GATE: {
			auto node = std::static_pointer_cast<activity::XorGate>(current_node);
			auto next_id = node->get_default()->get_id();
			for (auto &condition : node->get_conditions()) {
				auto condition_func = condition.second;
				if (condition_func(start_time, entity)) {
					next_id = condition.first;
					break;
				}
			}
			current_node = node->next(next_id);
		} break;
		case activity::node_t::XOR_EVENT_GATE: {
			auto node = std::static_pointer_cast<activity::XorEventGate>(current_node);
			auto event_primers = node->get_primers();
			for (auto &primer : event_primers) {
				auto ev = primer.second(start_time + event_wait_time,
				                        entity,
				                        loop,
				                        state,
				                        primer.first);
				activity_component->add_event(ev);
			}

			// exit and wait for event
			event_wait_time = 0;
			stop = true;
		} break;
		default:
			throw Error{ERR << "Unhandled node type for node " << current_node->str()};
		}
	}

	// save the current node in the component
	activity_component->set_node(start_time, current_node);
}

const time::time_t Activity::handle_subsystem(const time::time_t &start_time,
                                              const std::shared_ptr<gamestate::GameEntity> &entity,
                                              const std::shared_ptr<openage::gamestate::GameState> &state,
                                              system_id_t system_id) {
	switch (system_id) {
	case system_id_t::IDLE:
		return Idle::idle(entity, start_time);
		break;
	case system_id_t::MOVE_COMMAND:
		return Move::move_command(entity, state, start_time);
		break;
	case system_id_t::MOVE_DEFAULT:
		// TODO: replace destination value with a parameter
		return Move::move_default(entity, state, {1, 1, 1}, start_time);
		break;
	default:
		throw Error{ERR << "Unhandled subsystem " << static_cast<int>(system_id)};
	}

	return time::time_t::from_int(0);
}


} // namespace openage::gamestate::system
