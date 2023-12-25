// Copyright 2023-2023 the openage authors. See copying.md for legal info.

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

bool APIActivityEvent::is_event(const nyan::Object &obj) {
	nyan::fqon_t immediate_parent = obj.get_parents()[0];
	return immediate_parent == "engine.util.activity.event.Event";
}

activity::event_primer_t APIActivityEvent::get_primer(const nyan::Object &event) {
	return ACTIVITY_EVENT_PRIMERS.get(event.get_name());
}

} // namespace openage::gamestate::api
