// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "xor_node.h"


namespace openage::gamestate::activity {

ConditionNode::ConditionNode(node_id id,
                             const std::vector<std::shared_ptr<Node>> &outputs,
                             condition_func_t condition_func,
                             node_label label) :
	Node{id, label, outputs},
	condition_func{condition_func} {
}

const std::shared_ptr<Node> &ConditionNode::visit(const curve::time_t &time) const {
	return this->outputs.at(this->condition_func(time));
}

} // namespace openage::gamestate::activity
