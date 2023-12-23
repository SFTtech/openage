// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "xor_gate.h"

#include <unordered_map>


namespace openage::gamestate::activity {

XorGate::XorGate(node_id_t id,
                 node_label_t label) :
	Node{id, label, {}},
	conditions{},
	default_node{nullptr} {
}

XorGate::XorGate(node_id_t id,
                 node_label_t label,
                 const std::vector<std::shared_ptr<Node>> &outputs,
                 const std::vector<condition_t> &conditions,
                 const std::shared_ptr<Node> &default_node) :
	Node{id, label, outputs},
	conditions{},
	default_node{default_node} {
	if (conditions.size() != outputs.size()) [[unlikely]] {
		throw Error{MSG(err) << "XorGate " << this->str() << " has " << outputs.size()
		                     << " outputs but " << conditions.size() << " conditions"};
	}

	for (size_t i = 0; i < conditions.size(); ++i) {
		this->conditions.emplace(outputs[i]->get_id(), conditions[i]);
	}
}

void XorGate::add_output(const std::shared_ptr<Node> &output,
                         const condition_t condition_func) {
	this->outputs.emplace(output->get_id(), output);
	this->conditions.emplace(output->get_id(), condition_func);
}

const std::map<node_id_t, condition_t> &XorGate::get_conditions() const {
	return this->conditions;
}

const std::shared_ptr<Node> &XorGate::get_default() const {
	return this->default_node;
}

void XorGate::set_default(const std::shared_ptr<Node> &node) {
	if (this->default_node != nullptr) {
		throw Error{MSG(err) << "XorGate " << this->str() << " already has a default node"};
	}

	this->outputs.emplace(node->get_id(), node);
	this->default_node = node;
}

} // namespace openage::gamestate::activity
