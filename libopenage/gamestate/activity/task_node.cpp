// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "task_node.h"


namespace openage::gamestate::activity {

TaskNode::TaskNode(node_id id,
                   const std::shared_ptr<Node> &output,
                   task_func_t task,
                   node_label label) :
	Node{id, label, {output}},
	task{task} {
}

const std::shared_ptr<Node> &TaskNode::visit(const curve::time_t &time) const {
	this->task(time);

	return this->outputs.at(0);
}


} // namespace openage::gamestate::activity
