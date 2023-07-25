// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "activity.h"

#include "log/log.h"

#include "gamestate/activity/activity.h"
#include "gamestate/activity/event_node.h"
#include "gamestate/activity/start_node.h"
#include "gamestate/activity/task_node.h"
#include "gamestate/activity/task_system_node.h"
#include "gamestate/activity/xor_node.h"
#include "gamestate/component/internal/activity.h"
#include "gamestate/game_entity.h"

#include "gamestate/system/idle.h"
#include "gamestate/system/move.h"


namespace openage::gamestate::system {


void Activity::advance(const std::shared_ptr<gamestate::GameEntity> &entity,
                       const time::time_t &start_time,
                       const std::shared_ptr<openage::event::EventLoop> &loop,
                       const std::shared_ptr<openage::gamestate::GameState> &state) {
	auto activity_component = std::dynamic_pointer_cast<component::Activity>(
		entity->get_component(component::component_t::ACTIVITY));
	auto current_node = activity_component->get_node(start_time);

	if (current_node == nullptr) [[unlikely]] {
		throw Error{ERR << "No node defined in activity graph for entity "
		                << std::to_string(entity->get_id()) << " (t=" << start_time << ")"};
	}

	// TODO: this check should be moved to a more general pre-processing section
	if (current_node->get_type() == activity::node_t::EVENT_GATEWAY) {
		// returning to a event gateway means that the event has been triggered
		// move to the next node here
		auto node = std::static_pointer_cast<activity::EventNode>(current_node);
		auto event_next = node->get_next_func();
		auto next_id = event_next(start_time, entity, loop, state);
		current_node = node->next(next_id);

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
		case activity::node_t::TASK: {
			auto node = std::static_pointer_cast<activity::TaskNode>(current_node);
			auto task = node->get_task_func();
			task(start_time, entity);
			auto next_id = node->get_next();
			current_node = node->next(next_id);
		} break;
		case activity::node_t::TASK_SYSTEM: {
			auto node = std::static_pointer_cast<activity::TaskSystemNode>(current_node);
			auto task = node->get_system_id();
			event_wait_time = Activity::handle_subsystem(entity, start_time, task);
			auto next_id = node->get_next();
			current_node = node->next(next_id);
		} break;
		case activity::node_t::XOR_GATEWAY: {
			auto node = std::static_pointer_cast<activity::ConditionNode>(current_node);
			auto condition = node->get_condition_func();
			auto next_id = condition(start_time, entity);
			current_node = node->next(next_id);
		} break;
		case activity::node_t::EVENT_GATEWAY: {
			auto node = std::static_pointer_cast<activity::EventNode>(current_node);
			auto event_primer = node->get_primer_func();
			auto evs = event_primer(start_time + event_wait_time, entity, loop, state);
			for (auto &ev : evs) {
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

const time::time_t Activity::handle_subsystem(const std::shared_ptr<gamestate::GameEntity> &entity,
                                               const time::time_t &start_time,
                                               system_id_t system_id) {
	switch (system_id) {
	case system_id_t::IDLE:
		return Idle::idle(entity, start_time);
		break;
	case system_id_t::MOVE_COMMAND:
		return Move::move_command(entity, start_time);
		break;
	case system_id_t::MOVE_DEFAULT:
		return Move::move_default(entity, {1, 1, 1}, start_time);
		break;
	default:
		throw Error{ERR << "Unhandled subsystem " << static_cast<int>(system_id)};
	}

	return time::time_t::from_int(0);
}


} // namespace openage::gamestate::system
