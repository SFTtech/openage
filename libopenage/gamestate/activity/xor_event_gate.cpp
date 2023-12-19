// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#include "xor_event_gate.h"

#include <unordered_map>


namespace openage::gamestate::activity {

XorEventGate::XorEventGate(node_id_t id,
                           node_label_t label) :
	Node{id, label},
	primers{} {
}

XorEventGate::XorEventGate(node_id_t id,
                           node_label_t label,
                           const std::vector<std::shared_ptr<Node>> &outputs,
                           const std::map<node_id_t, event_primer_t> &primers) :
	Node{id, label, outputs},
	primers{} {
	if (primers.size() != outputs.size()) {
		throw Error{MSG(err) << "XorEventGate " << this->str() << " has " << outputs.size()
		                     << " outputs but " << primers.size() << " primers"};
	}

	for (const auto &[id, primer] : primers) {
		this->primers.emplace(id, primer);
	}
}

void XorEventGate::add_output(const std::shared_ptr<Node> &output,
                              const event_primer_t &primer) {
	this->outputs.emplace(output->get_id(), output);
	this->primers.emplace(output->get_id(), primer);
}

const std::map<node_id_t, event_primer_t> &XorEventGate::get_primers() const {
	return this->primers;
}

} // namespace openage::gamestate::activity
