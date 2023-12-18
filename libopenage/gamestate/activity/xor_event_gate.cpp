// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "xor_event_gate.h"

#include <unordered_map>


namespace openage::gamestate::activity {

XorEventGate::XorEventGate(node_id_t id,
                           node_label_t label,
                           const std::vector<std::shared_ptr<Node>> &outputs,
                           event_primer_func_t primer_func,
                           event_next_func_t next_func) :
	Node{id, label, outputs},
	primer_func{primer_func},
	next_func{next_func} {
}

XorEventGate::XorEventGate(node_id_t id,
                           node_label_t label) :
	Node{id, label},
	primer_func{no_event},
	next_func{no_next},
	primers{} {
}

XorEventGate::XorEventGate(node_id_t id,
                           node_label_t label,
                           const std::vector<std::shared_ptr<Node>> &outputs,
                           const std::map<node_id_t, event_primer_t> &primers) :
	Node{id, label, outputs},
	primer_func{no_event},
	next_func{no_next},
	primers{} {
	if (primers.size() != outputs.size()) {
		throw Error{MSG(err) << "XorEventGate " << this->str() << " has " << outputs.size()
		                     << " outputs but " << primers.size() << " primers"};
	}

	for (const auto &[id, primer] : primers) {
		this->primers.emplace(id, primer);
	}
}

void XorEventGate::add_output(const std::shared_ptr<Node> &output) {
	this->outputs.emplace(output->get_id(), output);
}

void XorEventGate::add_output(const std::shared_ptr<Node> &output,
                              const event_primer_t &primer) {
	this->outputs.emplace(output->get_id(), output);
	this->primers.emplace(output->get_id(), primer);
}

void XorEventGate::set_primer_func(event_primer_func_t primer_func) {
	this->primer_func = primer_func;
}

void XorEventGate::set_next_func(event_next_func_t next_func) {
	this->next_func = next_func;
}

event_primer_func_t XorEventGate::get_primer_func() const {
	return this->primer_func;
}

event_next_func_t XorEventGate::get_next_func() const {
	return this->next_func;
}

const std::map<node_id_t, event_primer_t> &XorEventGate::get_primers() const {
	return this->primers;
}

} // namespace openage::gamestate::activity
