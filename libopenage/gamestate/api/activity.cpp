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
	case activity::node_t::TASK_SYSTEM:
	case activity::node_t::START: {
		auto next = node.get<nyan::ObjectValue>("Node.next");
		std::shared_ptr<nyan::View> db_view = node.get_view();
		return {db_view->get_object(next->get_name())};
	}
	// 1+ next nodes
	case activity::node_t::XOR_GATE:
	case activity::node_t::XOR_EVENT_GATE: {
		auto next = node.get<nyan::Dict>("Node.next");
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

} // namespace openage::gamestate::api
