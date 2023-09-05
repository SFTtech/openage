// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "xor_node.h"

#include <unordered_map>


namespace openage::gamestate::activity {

XorGate::XorGate(node_id id,
                             node_label label,
                             const std::vector<std::shared_ptr<Node>> &outputs,
                             condition_func_t condition_func) :
	Node{id, label, outputs},
	condition_func{condition_func} {
}

void XorGate::add_output(const std::shared_ptr<Node> &output) {
	this->outputs.emplace(output->get_id(), output);
}

void XorGate::set_condition_func(condition_func_t condition_func) {
	this->condition_func = condition_func;
}

condition_func_t XorGate::get_condition_func() const {
	return this->condition_func;
}

} // namespace openage::gamestate::activity
