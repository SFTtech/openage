// Copyright 2023-2025 the openage authors. See copying.md for legal info.

#include "activity.h"

#include "gamestate/api/definitions.h"
#include "gamestate/api/util.h"


namespace openage::gamestate::api {

bool APIActivity::is_activity(const nyan::Object &obj) {
	for (auto &parent : obj.get_parents()) {
		if (parent == "engine.util.activity.Activity") {
			return true;
		}
	}

	return false;
}

nyan::Object APIActivity::get_start(const nyan::Object &activity) {
	auto obj_value = activity.get<nyan::ObjectValue>("Activity.start");

	std::shared_ptr<nyan::View> db_view = activity.get_view();
	return db_view->get_object(obj_value->get_name());
}


bool APIActivityNode::is_node(const nyan::Object &obj) {
	for (auto &parent : obj.get_parents()) {
		if (parent == "engine.util.activity.node.Node") {
			return true;
		}
	}

	return false;
}

activity::node_t APIActivityNode::get_type(const nyan::Object &node) {
	nyan::fqon_t api_parent = get_api_parent(node);

	return ACTIVITY_NODE_LOOKUP.get(api_parent);
}

std::vector<nyan::Object> APIActivityNode::get_next(const nyan::Object &node) {
	switch (APIActivityNode::get_type(node)) {
	// 0 next nodes
	case activity::node_t::END: {
		return {};
	}
	// 1 next node
	case activity::node_t::TASK_SYSTEM: {
		auto api_parent = get_api_parent(node);

		nyan::memberid_t member_name;
		if (api_parent == "engine.util.activity.node.type.Ability") {
			member_name = "Ability.next";
		}
		else if (api_parent == "engine.util.activity.node.type.Task") {
			member_name = "Task.next";
		}
		else {
			throw Error(MSG(err) << "Node type '" << api_parent << "' cannot be used to get the next node.");
		}

		auto next = node.get<nyan::ObjectValue>(member_name);
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
		auto switch_condition_parent = get_api_parent(switch_condition_obj);
		auto switch_condition_type = ACTIVITY_SWITCH_CONDITION_TYPE_LOOKUP.get(switch_condition_parent);

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

system::system_id_t APIActivityNode::get_system_id(const nyan::Object &node) {
	nyan::fqon_t api_parent = get_api_parent(node);

	nyan::fqon_t task_obj_fqon;
	if (api_parent == "engine.util.activity.node.type.Ability") {
		task_obj_fqon = node.get<nyan::ObjectValue>("Ability.ability")->get_name();
	}
	else if (api_parent == "engine.util.activity.node.type.Task") {
		task_obj_fqon = node.get<nyan::ObjectValue>("Task.task")->get_name();
	}
	else {
		throw Error(MSG(err) << "Node type '" << api_parent << "' cannot be used to get the system ID.");
	}

	// Get the API parent of the task object to look up the system ID
	auto view = node.get_view();
	auto task_obj = view->get_object(task_obj_fqon);
	task_obj_fqon = get_api_parent(task_obj);

	if (not ACTIVITY_TASK_SYSTEM_LOOKUP.contains(task_obj_fqon)) [[unlikely]] {
		throw Error(MSG(err) << "'" << task_obj.get_name() << "' has no associated system defined.");
	}

	return ACTIVITY_TASK_SYSTEM_LOOKUP.get(task_obj_fqon);
}

bool APIActivityCondition::is_condition(const nyan::Object &obj) {
	nyan::fqon_t api_parent = get_api_parent(obj);

	return api_parent == "engine.util.activity.condition.Condition";
}

activity::condition_t APIActivityCondition::get_condition(const nyan::Object &condition) {
	nyan::fqon_t api_parent = get_api_parent(condition);

	return ACTIVITY_CONDITION_LOOKUP.get(api_parent);
}

bool APIActivitySwitchCondition::is_switch_condition(const nyan::Object &obj) {
	nyan::fqon_t api_parent = get_api_parent(obj);

	return api_parent == "engine.util.activity.switch_condition.SwitchCondition";
}

activity::switch_function_t APIActivitySwitchCondition::get_switch_func(const nyan::Object &condition) {
	nyan::fqon_t api_parent = get_api_parent(condition);

	return ACTIVITY_SWITCH_CONDITION_LOOKUP.get(api_parent);
}

APIActivitySwitchCondition::lookup_map_t APIActivitySwitchCondition::get_lookup_map(const nyan::Object &condition) {
	nyan::fqon_t api_parent = get_api_parent(condition);

	auto switch_condition_type = ACTIVITY_SWITCH_CONDITION_TYPE_LOOKUP.get(api_parent);

	switch (switch_condition_type) {
	case switch_condition_t::NEXT_COMMAND: {
		auto next = condition.get<nyan::Dict>("NextCommand.next");
		lookup_map_t lookup_map{};
		for (auto next_node : next->get()) {
			auto key_value = std::dynamic_pointer_cast<nyan::ObjectValue>(next_node.first.get_ptr());
			auto key_obj = condition.get_view()->get_object(key_value->get_name());

			// Get engine lookup key value
			auto key = static_cast<activity::switch_key_t>(COMMAND_LOOKUP.get(key_obj.get_name()));

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
	nyan::fqon_t api_parent = get_api_parent(obj);

	return api_parent == "engine.util.activity.event.Event";
}

activity::event_primer_t APIActivityEvent::get_primer(const nyan::Object &event) {
	return ACTIVITY_EVENT_PRIMER_LOOKUP.get(event.get_name());
}

} // namespace openage::gamestate::api
