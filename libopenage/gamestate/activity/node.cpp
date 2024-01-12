// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "node.h"

#include <ostream>

#include "error/error.h"
#include "log/message.h"


namespace openage::gamestate::activity {

Node::Node(node_id_t id,
           node_label_t label,
           const std::vector<std::shared_ptr<Node>> &outputs) :
	outputs{},
	id{id},
	label{label} {
	this->outputs.reserve(outputs.size());
	for (const auto &output : outputs) {
		this->outputs.emplace(output->get_id(), output);
	}
}

node_id_t Node::get_id() const {
	return this->id;
}

const node_label_t Node::get_label() const {
	return this->label;
}

std::string Node::str() const {
	std::stringstream ret;
	if (this->label.empty()) {
		ret << this->get_id();
	}
	else {
		ret << this->get_label()
			<< " (id=" << this->get_id() << ")";
	}
	return ret.str();
}

const std::shared_ptr<Node> &Node::next(node_id_t id) const {
	if (not this->outputs.contains(id)) [[unlikely]] {
		throw Error{MSG(err) << "Node " << this->str() << " has no output with id " << id};
	}

	return this->outputs.at(id);
}

} // namespace openage::gamestate::activity
