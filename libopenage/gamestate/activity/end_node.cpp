#include "end_node.h"


namespace openage::gamestate::activity {

EndNode::EndNode(node_id id,
                 const std::shared_ptr<Node> &output,
                 node_label label) :
	Node{id, label, {}} {
	if (output) {
		this->outputs.emplace(output->get_id(), output);
	}
}

const std::shared_ptr<Node> &EndNode::visit(const curve::time_t & /* time */) const {
	if (this->outputs.empty()) {
		return {nullptr};
	}

	return this->outputs.at(0);
}

} // namespace openage::gamestate::activity
