// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#include "xor_switch_gate.h"


namespace openage::gamestate::activity {

XorSwitchGate::XorSwitchGate(node_id_t id,
                             node_label_t label) :
	Node{id, label} {}

XorSwitchGate::XorSwitchGate(node_id_t id,
                             node_label_t label,
                             const lookup_function_t &lookup_func,
                             const lookup_dict_t &lookup_dict,
                             const std::shared_ptr<Node> &default_node) :
	Node{id, label},
	lookup_func{lookup_func},
	lookup_dict{lookup_dict},
	default_node{default_node} {}

void XorSwitchGate::set_output(const std::shared_ptr<Node> &output,
                               const lookup_key_t &key) {
	this->outputs.emplace(output->get_id(), output);
	this->lookup_dict.emplace(key, output);
}

const XorSwitchGate::lookup_function_t &XorSwitchGate::get_lookup_func() const {
	return this->lookup_func;
}

void XorSwitchGate::set_lookup_func(const lookup_function_t &lookup_func) {
	this->lookup_func = lookup_func;
}

const XorSwitchGate::lookup_dict_t &XorSwitchGate::get_lookup_dict() const {
	return this->lookup_dict;
}

const std::shared_ptr<Node> &XorSwitchGate::get_default() const {
	return this->default_node;
}

void XorSwitchGate::set_default(const std::shared_ptr<Node> &node) {
	if (this->default_node != nullptr) {
		throw Error{MSG(err) << this->str() << " already has a default node"};
	}

	this->outputs.emplace(node->get_id(), node);
	this->default_node = node;
}

} // namespace openage::gamestate::activity
