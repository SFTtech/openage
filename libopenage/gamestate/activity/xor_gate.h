// Copyright 2023-2025 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <vector>

#include "error/error.h"
#include "log/message.h"

#include "gamestate/activity/node.h"
#include "gamestate/activity/types.h"
#include "time/time.h"


namespace openage::gamestate::activity {

/**
 * Chooses one of its output nodes based on conditions.
 */
class XorGate : public Node {
public:
	/**
	 * Creates a new condition node.
	 *
	 * @param id Unique identifier of the node.
	 * @param label Label of the node (optional).
	 */
	XorGate(node_id_t id,
	        node_label_t label = "ExclusiveGateway");

	/**
	 * Creates a new condition node.
	 *
	 * @param id Unique identifier of the node.
	 * @param label Label of the node.
	 * @param outputs Output nodes.
	 * @param conditions Conditions for each output node.
	 * @param default_node Default output node. Chosen if no condition is true.
	 */
	XorGate(node_id_t id,
	        node_label_t label,
	        const std::vector<std::shared_ptr<Node>> &outputs,
	        const std::vector<condition> &conditions,
	        const std::shared_ptr<Node> &default_node);

	virtual ~XorGate() = default;

	inline node_t get_type() const override {
		return node_t::XOR_GATE;
	}

	/**
	 * Add an output node.
	 *
	 * @param output Output node.
	 * @param condition Function that determines whether this output node is chosen.
	 *                       This must be a valid node ID of one of the output nodes.
	 */
	void add_output(const std::shared_ptr<Node> &output,
	                const condition condition);

	/**
	 * Get the output->condition mappings.
	 *
	 * @return Conditions for each output node.
	 */
	const std::map<node_id_t, condition> &get_conditions() const;

	/**
	 * Get the default output node.
	 *
	 * @return Default output node.
	 */
	const std::shared_ptr<Node> &get_default() const;

	/**
	 * Set the the default output node.
	 *
	 * This node is chosen if no condition is true.
	 *
	 * @param node Default output node.
	 */
	void set_default(const std::shared_ptr<Node> &node);

private:
	/**
	 * Maps output node IDs to conditions.
	 *
	 * Conditions are checked in order they appear in the map.
	 */
	std::map<node_id_t, condition> conditions;

	/**
	 * Default output node. Chosen if no condition is true.
	 */
	std::shared_ptr<Node> default_node;
};

} // namespace openage::gamestate::activity
