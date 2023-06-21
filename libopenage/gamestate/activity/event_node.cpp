#include "event_node.h"


namespace openage::gamestate::activity {

EventNode::EventNode(node_id id,
                     const std::vector<std::shared_ptr<Node>> &outputs,
                     event_primer_func_t primer_func,
                     event_next_func_t next_func,
                     node_label label) :
	Node{id, label, outputs},
	primer_func{primer_func},
	next_func{next_func} {
}

const std::shared_ptr<Node> &EventNode::visit(const curve::time_t &time) const {
	this->primer_func(time);
	return {nullptr};
}

const std::shared_ptr<Node> &EventNode::next(const curve::time_t &time) const {
	return this->outputs.at(this->next_func(time));
}

} // namespace openage::gamestate::activity
