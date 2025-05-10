// Copyright 2023-2025 the openage authors. See copying.md for legal info.

#include "activity.h"

#include "gamestate/api/definitions.h"


namespace openage::gamestate::api {

bool APIActivity::is_activity(const nyan::Object &obj) {
	nyan::fqon_t immediate_parent = obj.get_parents()[0];
	return immediate_parent == "engine.util.activity.Activity";
}

nyan::Object APIActivity::get_start(const nyan::Object &activity) {
	auto obj_value = activity.get<nyan::ObjectValue>("Activity.start");

	std::shared_ptr<nyan::View> db_view = activity.get_view();
	return db_view->get_object(obj_value->get_name());
}


bool APIActivityNode::is_node(const nyan::Object &obj) {
	nyan::fqon_t immediate_parent = obj.get_parents()[0];
	return immediate_parent == "engine.util.activity.node.Node";
}

activity::node_t APIActivityNode::get_type(const nyan::Object &node) {
	nyan::fqon_t immediate_parent = node.get_parents()[0];
	return ACTIVITY_NODE_DEFS.get(immediate_parent);
}

std::vector<nyan::Object> APIActivityNode::get_next(const nyan::Object &node) {
	switch (APIActivityNode::get_type(node)) {
	// 0 next nodes
	case activity::node_t::END: {
		return {};
	}
	// 1 next node
	case activity::node_t::TASK_SYSTEM: {
		auto next = node.get<nyan::ObjectValue>("Ability.next");
		std::shared_ptr<nyan::View> db_view = node.get_view();
		return {db_view->get_object(next->get_name())};
	}
	case activity::node_t::START: {
		auto next = node.get<nyan::ObjectValue>("Start.next");
		std::shared_ptr<nyan::View> db_view = node.get_view();
		return {db_view->get_object(next->get_name())};
	}
	// 1+ next nodes
	case activity::node_t::XOR_GATE: {
		auto conditions = node.get<nyan::OrderedSet>("XORGate.next");
		std::shared_ptr<nyan::View> db_view = node.get_view();

		std::vector<nyan::Object> next_nodes;
		for (auto &condition : conditions->get()) {
			auto condition_value = std::dynamic_pointer_cast<nyan::ObjectValue>(condition.get_ptr());
			auto condition_obj = db_view->get_object(condition_value->get_name());

			auto next_node_value = condition_obj.get<nyan::ObjectValue>("Condition.next");
			next_nodes.push_back(db_view->get_object(next_node_value->get_name()));
		}

		auto default_next = node.get<nyan::ObjectValue>("XORGate.default");
		next_nodes.push_back(db_view->get_object(default_next->get_name()));

		return next_nodes;
	}
	case activity::node_t::XOR_EVENT_GATE: {
		auto next = node.get<nyan::Dict>("XOREventGate.next");
		std::shared_ptr<nyan::View> db_view = node.get_view();

		std::vector<nyan::Object> next_nodes;
		for (auto &next_node : next->get()) {
			auto next_node_value = std::dynamic_pointer_cast<nyan::ObjectValue>(next_node.second.get_ptr());
			next_nodes.push_back(db_view->get_object(next_node_value->get_name()));
		}

		return next_nodes;
	}
	case activity::node_t::XOR_SWITCH_GATE: {
		auto switch_condition = node.get<nyan::ObjectValue>("XORSwitchGate.switch");
		std::shared_ptr<nyan::View> db_view = node.get_view();

		auto switch_condition_obj = db_view->get_object(switch_condition->get_name());
		auto switch_condition_parent = switch_condition_obj.get_parents()[0];
		auto switch_condition_type = ACTIVITY_SWITCH_CONDITION_TYPES.get(switch_condition_parent);

		switch (switch_condition_type) {
		case switch_condition_t::NEXT_COMMAND: {
			auto next = switch_condition_obj.get_dict("NextCommand.next");
			std::vector<nyan::Object> next_nodes;
			for (auto next_node : next) {
				auto next_node_value = std::dynamic_pointer_cast<nyan::ObjectValue>(next_node.second.get_ptr());
				next_nodes.push_back(db_view->get_object(next_node_value->get_name()));
			}

			return next_nodes;
		}
		default:
			throw Error(MSG(err) << "Unknown switch condition type.");
		}
	}
	default:
		throw Error(MSG(err) << "Unknown activity node type.");
	}
}

system::system_id_t APIActivityNode::get_system_id(const nyan::Object &ability_node) {
	auto ability = ability_node.get<nyan::ObjectValue>("Ability.ability");

	if (not ACTIVITY_TASK_SYSTEM_DEFS.contains(ability->get_name())) [[unlikely]] {
		throw Error(MSG(err) << "Ability '" << ability->get_name() << "' has no associated system defined.");
	}

	return ACTIVITY_TASK_SYSTEM_DEFS.get(ability->get_name());
}

bool APIActivityCondition::is_condition(const nyan::Object &obj) {
	nyan::fqon_t immediate_parent = obj.get_parents()[0];
	return immediate_parent == "engine.util.activity.condition.Condition";
}

activity::condition_t APIActivityCondition::get_condition(const nyan::Object &condition) {
	nyan::fqon_t immediate_parent = condition.get_parents()[0];
	return ACTIVITY_CONDITIONS.get(immediate_parent);
}

bool APIActivitySwitchCondition::is_switch_condition(const nyan::Object &obj) {
	nyan::fqon_t immediate_parent = obj.get_parents()[0];
	return immediate_parent == "engine.util.activity.switch_condition.SwitchCondition";
}

activity::switch_function_t APIActivitySwitchCondition::get_switch_func(const nyan::Object &condition) {
	nyan::fqon_t immediate_parent = condition.get_parents()[0];
	return ACTIVITY_SWITCH_CONDITIONS.get(immediate_parent);
}

APIActivitySwitchCondition::lookup_map_t APIActivitySwitchCondition::get_lookup_map(const nyan::Object &condition) {
	nyan::fqon_t immediate_parent = condition.get_parents()[0];
	auto switch_condition_type = ACTIVITY_SWITCH_CONDITION_TYPES.get(immediate_parent);

	switch (switch_condition_type) {
	case switch_condition_t::NEXT_COMMAND: {
		auto next = condition.get<nyan::Dict>("NextCommand.next");
		lookup_map_t lookup_map{};
		for (auto next_node : next->get()) {
			auto key_value = std::dynamic_pointer_cast<nyan::ObjectValue>(next_node.first.get_ptr());
			auto key_obj = condition.get_view()->get_object(key_value->get_name());

			// Get engine lookup key value
			auto key = static_cast<activity::switch_key_t>(COMMAND_DEFS.get(key_obj.get_name()));

			// Get node ID
			auto next_node_value = std::dynamic_pointer_cast<nyan::ObjectValue>(next_node.second.get_ptr());
			auto next_node_id = next_node_value->get_name();

			lookup_map[key] = next_node_id;
		}

		return lookup_map;
	}
	default:
		throw Error(MSG(err) << "Unknown switch condition type.");
	}
}

bool APIActivityEvent::is_event(const nyan::Object &obj) {
	nyan::fqon_t immediate_parent = obj.get_parents()[0];
	return immediate_parent == "engine.util.activity.event.Event";
}

activity::event_primer_t APIActivityEvent::get_primer(const nyan::Object &event) {
	return ACTIVITY_EVENT_PRIMERS.get(event.get_name());
}

} // namespace openage::gamestate::api
