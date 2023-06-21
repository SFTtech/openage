#include "event_node.h"


namespace openage::gamestate::activity {

EventNode::EventNode(node_id id,
                     node_label label,
                     const std::vector<std::shared_ptr<Node>> &outputs,
                     event_primer_func_t primer_func,
                     event_next_func_t next_func) :
	Node{id, label, outputs},
	primer_func{primer_func},
	next_func{next_func} {
}

void EventNode::add_output(const std::shared_ptr<Node> &output) {
	this->outputs.emplace(output->get_id(), output);
}

void EventNode::set_primer_func(event_primer_func_t primer_func) {
	this->primer_func = primer_func;
}

void EventNode::set_next_func(event_next_func_t next_func) {
	this->next_func = next_func;
}

const std::shared_ptr<Node> &EventNode::visit(const curve::time_t &time) const {
	this->primer_func(time);
	return {nullptr};
}

const std::shared_ptr<Node> &EventNode::next(const curve::time_t &time) const {
	if (this->outputs.empty()) {
		throw Error{MSG(err) << "Event node " << this->str() << " has no outputs."};
	}

	return this->outputs.at(this->next_func(time));
}

} // namespace openage::gamestate::activity
