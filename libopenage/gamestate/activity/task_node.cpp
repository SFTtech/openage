// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "task_node.h"


namespace openage::gamestate::activity {

TaskNode::TaskNode(node_id id,
                   node_label label,
                   const std::shared_ptr<Node> &output,
                   task_func_t task_func) :
	Node{id, label},
	task_func{task_func} {
	if (output) {
		this->add_output(output);
	}
}

void TaskNode::add_output(const std::shared_ptr<Node> &output) {
	this->outputs.clear();
	this->outputs.emplace(output->get_id(), output);
}

void TaskNode::set_task_func(task_func_t task_func) {
	this->task_func = task_func;
}

const std::shared_ptr<Node> &TaskNode::visit(const curve::time_t &time) const {
	if (this->outputs.empty()) [[unlikely]] {
		throw Error{MSG(err) << "Task node " << this->str() << " has no output."};
	}

	this->task_func(time);

	return (*this->outputs.begin()).second;
}


} // namespace openage::gamestate::activity