// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "start_node.h"


namespace openage::gamestate::activity {

StartNode::StartNode(node_id id,
                     const std::shared_ptr<Node> &output,
                     node_label label) :
	Node{id, label, {output}} {
}

const std::shared_ptr<Node> &StartNode::visit(const curve::time_t & /* time */) const {
	return this->outputs.at(0);
}

} // namespace openage::gamestate::activity
