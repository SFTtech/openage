// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "task_node.h"

#include <unordered_map>
#include <utility>


namespace openage::gamestate::activity {

TaskCustom::TaskCustom(node_id_t id,
                   node_label_t label,
                   const std::shared_ptr<Node> &output,
                   task_func_t task_func) :
	Node{id, label},
	task_func{task_func} {
	if (output) {
		this->add_output(output);
	}
}

void TaskCustom::add_output(const std::shared_ptr<Node> &output) {
	this->outputs.clear();
	this->outputs.emplace(output->get_id(), output);
}

void TaskCustom::set_task_func(task_func_t task_func) {
	this->task_func = task_func;
}

task_func_t TaskCustom::get_task_func() const {
	return this->task_func;
}

node_id_t TaskCustom::get_next() const {
	return (*this->outputs.begin()).first;
}

} // namespace openage::gamestate::activity
