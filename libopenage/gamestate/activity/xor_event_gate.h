// Copyright 2023-2025 the openage authors. See copying.md for legal info.

#pragma once

#include <map>
#include <memory>
#include <vector>

#include "error/error.h"
#include "log/message.h"

#include "gamestate/activity/node.h"
#include "gamestate/activity/types.h"
#include "time/time.h"


namespace openage::gamestate::activity {

/**
 * Waits for an event to be executed before continuing the control flow.
 */
class XorEventGate : public Node {
public:
	/**
	 * Create a new exclusive event gateway.
	 *
	 * @param id Unique identifier for this node.
	 * @param label Human-readable label (optional).
	 */
	XorEventGate(node_id_t id,
	             node_label_t label = "ExclusiveEventGateway");

	/**
	 * Create a new exclusive event gateway.
	 *
	 * @param id Unique identifier for this node.
	 * @param label Human-readable label.
	 * @param outputs Output nodes.
	 * @param primers Event primers for each output node.
	 */
	XorEventGate(node_id_t id,
	             node_label_t label,
	             const std::vector<std::shared_ptr<Node>> &outputs,
	             const std::map<node_id_t, event_primer_t> &primers);

	virtual ~XorEventGate() = default;

	inline node_t get_type() const override {
		return node_t::XOR_EVENT_GATE;
	}

	/**
	 * Add an output node.
	 *
	 * @param output Output node.
	 * @param primer Creation function for the event associated with the output node.
	 */
	void add_output(const std::shared_ptr<Node> &output,
	                const event_primer_t &primer);

	/**
	 * Get the output->event primer mappings.
	 *
	 * @return Event primer functions for each output node.
	 */
	const std::map<node_id_t, event_primer_t> &get_primers() const;

private:
	/**
	 * Maps output node IDs to event primer functions.
	 *
	 * Events are created and registered on the event loop when the node is visited.
	 */
	std::map<node_id_t, event_primer_t> primers;
};

} // namespace openage::gamestate::activity
