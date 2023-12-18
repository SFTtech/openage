// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "xor_gate.h"

#include <unordered_map>


namespace openage::gamestate::activity {

XorGate::XorGate(node_id_t id,
                 node_label_t label,
                 const std::vector<std::shared_ptr<Node>> &outputs,
                 condition_func_t condition_func) :
	Node{id, label, outputs},
	condition_func{condition_func},
	conditions{},
	default_id{std::nullopt} {
}

XorGate::XorGate(node_id_t id,
                 node_label_t label,
                 const std::vector<std::shared_ptr<Node>> &outputs,
                 const std::vector<condition_t> &conditions,
                 const node_id_t default_id) :
	Node{id, label, outputs},
	condition_func{no_condition},
	conditions{},
	default_id{std::nullopt} {
	if (conditions.size() != outputs.size()) [[unlikely]] {
		throw Error{MSG(err) << "XorGate " << this->str() << " has " << outputs.size()
		                     << " outputs but " << conditions.size() << " conditions"};
	}

	for (size_t i = 0; i < conditions.size(); ++i) {
		this->conditions.emplace(outputs[i]->get_id(), conditions[i]);
	}

	this->set_default_id(default_id);
}

void XorGate::add_output(const std::shared_ptr<Node> &output) {
	this->outputs.emplace(output->get_id(), output);
}

void XorGate::add_output(const std::shared_ptr<Node> &output,
                         const condition_t condition_func) {
	this->outputs.emplace(output->get_id(), output);
	this->conditions.emplace(output->get_id(), condition_func);

	// If this is the first output, set it as the default.
	if (not this->default_id) [[unlikely]] {
		this->default_id = output->get_id();
	}
}

void XorGate::set_condition_func(condition_func_t condition_func) {
	this->condition_func = condition_func;
}

condition_func_t XorGate::get_condition_func() const {
	return this->condition_func;
}

const std::map<node_id_t, condition_t> &XorGate::get_conditions() const {
	return this->conditions;
}

node_id_t XorGate::get_default_id() const {
	if (not this->default_id) [[unlikely]] {
		throw Error{MSG(err) << "XorGate " << this->str() << " has no default output"};
	}

	return this->default_id.value();
}

void XorGate::set_default_id(node_id_t id) {
	if (not this->outputs.contains(id)) [[unlikely]] {
		throw Error{MSG(err) << "XorGate " << this->str() << " has no output with id " << id};
	}

	this->default_id = id;
}

} // namespace openage::gamestate::activity
