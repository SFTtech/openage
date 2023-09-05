// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "start_node.h"

#include <unordered_map>
#include <utility>


namespace openage::gamestate::activity {

StartNode::StartNode(node_id id,
                     node_label label,
                     const std::shared_ptr<Node> &output) :
	Node{id, label} {
	if (output) {
		this->add_output(output);
	}
}

void StartNode::add_output(const std::shared_ptr<Node> &output) {
	this->outputs.clear();
	this->outputs.emplace(output->get_id(), output);
}

node_id StartNode::get_next() const {
	return (*this->outputs.begin()).first;
}

} // namespace openage::gamestate::activity
