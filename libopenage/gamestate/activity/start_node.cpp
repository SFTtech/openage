// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "start_node.h"


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

const std::shared_ptr<Node> &StartNode::visit(const curve::time_t & /* time */) const {
	if (this->outputs.empty()) [[unlikely]] {
		throw Error{MSG(err) << "Start node " << this->str() << " has no output."};
	}

	return (*this->outputs.begin()).second;
}

} // namespace openage::gamestate::activity
