// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "task_system_node.h"

#include <unordered_map>
#include <utility>


namespace openage::gamestate::activity {

TaskSystemNode::TaskSystemNode(node_id_t id,
                               node_label_t label,
                               const std::shared_ptr<Node> &output,
                               system::system_id_t system_id) :
	Node{id, label},
	system_id{system_id} {
	if (output) {
		this->add_output(output);
	}
}

void TaskSystemNode::add_output(const std::shared_ptr<Node> &output) {
	this->outputs.clear();
	this->outputs.emplace(output->get_id(), output);
}

void TaskSystemNode::set_system_id(system::system_id_t system_id) {
	this->system_id = system_id;
}

system::system_id_t TaskSystemNode::get_system_id() const {
	return this->system_id;
}

node_id_t TaskSystemNode::get_next() const {
	return (*this->outputs.begin()).first;
}

} // namespace openage::gamestate::activity
