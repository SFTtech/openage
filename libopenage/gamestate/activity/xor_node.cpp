// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "xor_node.h"


namespace openage::gamestate::activity {

ConditionNode::ConditionNode(node_id id,
                             node_label label,
                             const std::vector<std::shared_ptr<Node>> &outputs,
                             condition_func_t condition_func) :
	Node{id, label, outputs},
	condition_func{condition_func} {
}

void ConditionNode::add_output(const std::shared_ptr<Node> &output) {
	this->outputs.emplace(output->get_id(), output);
}

void ConditionNode::set_condition_func(condition_func_t condition_func) {
	this->condition_func = condition_func;
}

const std::shared_ptr<Node> &ConditionNode::visit(const curve::time_t &time) const {
	if (this->outputs.empty()) [[unlikely]] {
		throw Error{MSG(err) << "Condition node " << this->str() << " has no outputs."};
	}

	return this->outputs.at(this->condition_func(time));
}

} // namespace openage::gamestate::activity
