// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "end_node.h"


namespace openage::gamestate::activity {

EndNode::EndNode(node_id id,
                 node_label label) :
	Node{id, label, {}} {
}

void EndNode::add_output(const std::shared_ptr<Node> &output) {
	throw Error{ERR << "End node cannot have outputs"};
}

const std::shared_ptr<Node> &EndNode::visit(const curve::time_t & /* time */) const {
	return {nullptr};
}

} // namespace openage::gamestate::activity
