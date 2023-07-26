// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "event_node.h"

#include <unordered_map>


namespace openage::gamestate::activity {

EventNode::EventNode(node_id id,
                     node_label label,
                     const std::vector<std::shared_ptr<Node>> &outputs,
                     event_primer_func_t primer_func,
                     event_next_func_t next_func) :
	Node{id, label, outputs},
	primer_func{primer_func},
	next_func{next_func} {
}

void EventNode::add_output(const std::shared_ptr<Node> &output) {
	this->outputs.emplace(output->get_id(), output);
}

void EventNode::set_primer_func(event_primer_func_t primer_func) {
	this->primer_func = primer_func;
}

void EventNode::set_next_func(event_next_func_t next_func) {
	this->next_func = next_func;
}

event_primer_func_t EventNode::get_primer_func() const {
	return this->primer_func;
}

event_next_func_t EventNode::get_next_func() const {
	return this->next_func;
}

} // namespace openage::gamestate::activity
